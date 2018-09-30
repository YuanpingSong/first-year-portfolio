//
//  server.c
//  P1B
//
//  Created by Yuanping Song on 7/13/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#define _POSIX_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <getopt.h>
#include <strings.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/poll.h>
#include <signal.h>
#include <string.h>
#include "zlib.h"

const int DEBUG_FLAG = 0;

void error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int compressMessage(char *msg, int n, char *buffer, int* buff_size) {
    z_stream compression_stream;
    compression_stream.zalloc = Z_NULL;
    compression_stream.zfree = Z_NULL;
    compression_stream.opaque = Z_NULL;
    compression_stream.next_in = (Bytef*) msg;
    compression_stream.avail_in = n;
    compression_stream.next_out = (Bytef*) buffer;
    compression_stream.avail_out = *buff_size;
    
    int rc = deflateInit(&compression_stream, 9);
    if (rc < 0) {
        deflateEnd(&compression_stream);
        error("deflate stream");
    }
    
    while (1) {
        deflate(&compression_stream, Z_SYNC_FLUSH);
        if (compression_stream.avail_in != 0) {
            if((buffer = realloc(buffer, *buff_size * 2)) == NULL) {
                deflateEnd(&compression_stream);
                error("realloc buffer");
            }
            *buff_size *= 2;
            compression_stream.next_in = (Bytef*) msg;
            compression_stream.avail_in = n;
            compression_stream.next_out = (Bytef*) buffer;
            compression_stream.avail_out = *buff_size;
            continue;
        }
        break;
    }
    
    
    return (int)( (void *)compression_stream.next_out - (void *)buffer); // number of chars written to buffer
}

int decompressMessage(char *msg, int n, char *buffer, int *buff_size) {
    z_stream decompression_stream;
    decompression_stream.zalloc = Z_NULL;
    decompression_stream.zfree = Z_NULL;
    decompression_stream.opaque = Z_NULL;
    decompression_stream.next_in = (Bytef*) msg;
    decompression_stream.avail_in = n;
    decompression_stream.next_out = (Bytef*) buffer;
    decompression_stream.avail_out = *buff_size;
    
    int rc = inflateInit(&decompression_stream);
    if (rc < 0) {
        inflateEnd(&decompression_stream);
        error("inflate stream");
    }
    
    while (1) {
        inflate(&decompression_stream, Z_SYNC_FLUSH);
        if (decompression_stream.avail_in != 0) {
            if((buffer = realloc(buffer, *buff_size * 2)) == NULL) {
                inflateEnd(&decompression_stream);
                error("realloc buffer");
            }
            *buff_size *= 2;
            decompression_stream.next_in = (Bytef*) msg;
            decompression_stream.avail_in = n;
            decompression_stream.next_out = (Bytef*) buffer;
            decompression_stream.avail_out = *buff_size;
            continue;
        }
        break;
    }
    
    return (int)( (void *)decompression_stream.next_out - (void *)buffer); // number of chars written to buffer
}

void sigpipe_handler(int signum) {
    fprintf(stdout, "sigpipe encountered: %d", signum);
    exit(EXIT_SUCCESS);
}

// error-sensitive sys calls
ssize_t sys_read(int fd, void *buf, size_t count, char *msg) {
    ssize_t c = read(fd, buf, count); if (c < 0) { error(msg); } return c; }

void sys_write(int fd, const void *buf, size_t n, char *msg) { if(write(fd, buf, n) < 0) { error(msg); } }

void sys_open(const char *pathname, int flags, char *msg) { if (open(pathname, flags) < 0) { error(msg); } }

void sys_close(int fd, char *msg) { if (close(fd) < 0) { error(msg); } }

void sys_pipe(int pipefd[2], char *msg) { if (pipe(pipefd) < 0) { error(msg); } }

void sys_waitpid(pid_t pid, int *status, int options, char *msg) { if(waitpid(pid, status, options) < 0) { error(msg); } }

int sys_fork(char *msg) {
    int rc = fork();
    if (rc < 0) { error(msg); }
    return rc;
}

// assumption: size greater than zero
void* sys_malloc(size_t size, char* msg) {
    void* buffer = malloc(size);
    if (buffer == NULL) {
        error(msg);
    }
    return buffer;
}

int main(int argc, char *argv[]) {
    // parse argument(s)
    static int compress_flag = 0;
    int *portno = NULL;
    char *shellPath = sys_malloc(strlen("/bin/bash") + 1, "initialize shellPath");
    strcpy(shellPath, "/bin/bash");
    
    static struct option long_options[] = {
        {"compress", no_argument, &compress_flag, 1},
        {"shell", required_argument, NULL, 's'},
        {"port", required_argument, NULL, 'p'},
        {0, 0, 0, 0} // last element of the array needs be filled this way
    };
    int opt;
    while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (opt) {
            case 0: // encountered flag
                break;
            case 's':
                free(shellPath);
                shellPath = sys_malloc(strlen(optarg) + 1, "shellPath from optarg");
                strcpy(shellPath, optarg);
                break;
            case 'p':
                portno = sys_malloc(sizeof(int), "malloc portno");
                *portno = atoi(optarg);
                break;
            default: // '?'
                error("Usage: ./server --port=# [--shell] [--compress]");
                break;
        }
    }
    if (portno == NULL) {
        fprintf(stderr, "Usage: ./server --port=# [--shell] [--compress]\n");
        exit(EXIT_FAILURE);
    }
    
    // buffer setup
    int BUFF_SIZE = 512, message_size = 4096;
    char *buffer = sys_malloc(BUFF_SIZE, "malloc buffer"); /* allocate a 1K buffer */
    char *message = sys_malloc(message_size, "malloc message");
    
    // configure socket
    int sockfd, newsockfd;
    unsigned clilen;
    struct sockaddr_in serv_addr, cli_addr;
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error("opening socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(*portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("binding");
    }
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    if((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0) {
        error("accept");
    }
    
    if (DEBUG_FLAG) {
        fprintf(stdout, "Connection Made!\n");
    }
    
    // Pipe setup
    int from_parent_to_child[2];
    int from_child_to_parent[2];
    sys_pipe(from_parent_to_child, "from parent to child pipe");
    sys_pipe(from_child_to_parent, "from child to parent pipe");
    
    // fork
    int rc = sys_fork("fork");
    
    if (rc == 0) { // child process goes here
        if (DEBUG_FLAG) {
            fprintf(stdout, "Forked a child!\n");
        }
        sys_close(from_child_to_parent[0], "In Child: from c to p read end"); // only write
        sys_close(from_parent_to_child[1], "In Child: from p to c write end"); // only read
        
        // redirect child's stdin
        if (close(STDIN_FILENO) < 0) { perror("In child: stdin"); exit(EXIT_FAILURE); }
        if (dup(from_parent_to_child[0]) < 0 ) { perror("In child: dup p to c read"); exit(EXIT_FAILURE); }
        if (close(from_parent_to_child[0]) < 0) { perror("In child: close p to c read"); exit(EXIT_FAILURE); }
        // redirect child's stdout & stderror
        if (close(STDOUT_FILENO) < 0) { perror("In child: stdout"); exit(EXIT_FAILURE); }
        if (dup(from_child_to_parent[1]) < 0) { perror("In child: dup c to p write for stdout"); exit(EXIT_FAILURE); }
        if (close(STDERR_FILENO) < 0) { perror("In child: stderr"); exit(EXIT_FAILURE); }
        if (dup(from_child_to_parent[1]) < 0) { perror("In child: dup c to p write for stderr"); exit(EXIT_FAILURE); }
        if (close(from_child_to_parent[1]) < 0) { perror("In child: close c to p write"); exit(EXIT_FAILURE); }
        
        if (DEBUG_FLAG) {
            fprintf(stdout, "Child Process about to exec!\n");
        }
        
        // execute shell
        if (execl("/bin/bash", "bash", 0) < 0) { // replace later
            perror("In child: exec bash");
            exit(EXIT_FAILURE);
        }
        
    } else {
        if (DEBUG_FLAG) {
            fprintf(stdout, "Parent goes on!\n");
        }
        sys_close(from_child_to_parent[1], "In Parent: from c to p write end"); // only read
        sys_close(from_parent_to_child[0], "In Parent: from p to c read end"); // only write
        
        signal(SIGPIPE, sigpipe_handler);
        
        struct pollfd fds[2];
        fds[0].fd = newsockfd;
        fds[1].fd = from_child_to_parent[0];
        fds[0].events = POLLIN | POLLERR | POLLHUP;
        fds[1].events = POLLIN | POLLERR | POLLHUP;
        
        char c, output[2] = {13, 10}; // cr lf
        int num_c, exit_flag = 0;
        while (1) {
            int p = poll(fds, 2, 0);
            if (p > 0) {
                // write to shell as is, excpet ^C, ^D
                if ((fds[0].revents & POLLIN) && !exit_flag) {
                    num_c = (int) sys_read(fds[0].fd, buffer, BUFF_SIZE, "read socket");
                    if (compress_flag) {
                        num_c = decompressMessage(buffer, num_c, message, &message_size);
                    }
                    char *temp = compress_flag ? message : buffer;
                    for (int i = 0; i < num_c; i++) {
                        c = temp[i];
                        switch (c) {
                            case 4: /* ^D */
                                sys_close(from_parent_to_child[1], "p2c write");
                                exit_flag = 1;
                                break;
                            case 3:
                                kill(rc, SIGINT);
                                break;
                            case 13:
                                sys_write(from_parent_to_child[1], &output[1], 1, "write lf to shell");
                                if (DEBUG_FLAG) {
                                    fprintf(stdout, "wrote to shell!\n");
                                }
                                break;
                            default:
                                sys_write(from_parent_to_child[1], &c, 1, "write to shell");
                                if (DEBUG_FLAG) {
                                    fprintf(stdout, "wrote to shell!\n");
                                }
                                break;
                        } // end switch
                    } // end for
                } // end socket
                // echo shell to socket
                if (fds[1].revents & POLLIN) {
                    int count = (int) sys_read(fds[1].fd, buffer, BUFF_SIZE, "read shell");
                    if (compress_flag) {
                        count = compressMessage(buffer, count, message, &BUFF_SIZE);
                        if (DEBUG_FLAG) {
                            fprintf(stdout, "count is %d\n", count);
                        }
                    }
                    char *temp = compress_flag ? message : buffer;
                    sys_write(fds[0].fd, temp, count, "write to socket");
                    if (DEBUG_FLAG) {
                        fprintf(stdout, "Wrote to socket!\n");
                    }
                } // end shell
                if (fds[1].revents & (POLLHUP | POLLERR)) { // shell exits
                    sys_close(fds[1].fd, "shell fds[1].fd");
                    sys_close(fds[0].fd, "close socket too");
                    int wstatus;
                    sys_waitpid(rc, &wstatus, 0, "parent waiting child");
                    WEXITSTATUS(wstatus);
                    fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(wstatus), WEXITSTATUS(wstatus));
                    break; // stop application
                }
                if ((fds[0].revents & (POLLHUP | POLLERR)) && !exit_flag) { // socket exits
                    sys_close(fds[0].fd, "socket");
                    sys_close(from_parent_to_child[1], "p to c write");
                    exit_flag = 1;
                    continue; // await shell to exit
                }
            } else if (p == 0) {
                continue;
            } else {
                error("Poll");
            }
        }
    }
    
    /*
    sys_read(newsockfd, buffer, BUFF_SIZE, "write to buffer");
    fprintf(stdout, "%s", buffer);
    sys_write(newsockfd, "acknowledged", 13, "write to socket");
    */
    
    // buffer cleanup
    free(buffer);
    free(shellPath);
    if (DEBUG_FLAG) {
        fprintf(stdout, "All done!\n");
    }
    return 0;
    
}

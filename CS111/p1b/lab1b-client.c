//
//  client.c
//  P1B
//
//  Created by Yuanping Song on 7/13/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
#include "zlib.h"
//#define _POSIX_SOURCE
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <poll.h>
#include <sys/stat.h>



#include <stdio.h>



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

const int DEBUG_FLAG = 0;

struct termios terminal_defaults;



void restore_term() {
    if (tcsetattr(STDIN_FILENO, TCSANOW, &terminal_defaults) == -1) {
        error("tcsetattr return to canonnical mode");
    }
}


// error-sensitive sys calls

void sys_read(int fd, void *buf, size_t count, char *msg) { if (read(fd, buf, count) < 0) { error(msg); } }

void sys_write(int fd, const void *buf, size_t n, char *msg) { if(write(fd, buf, n) < 0) { error(msg); } }

//void sys_open(const char *pathname, int flags, char *msg) { if (open(pathname, flags) < 0) { error(msg); } }

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

int main(int argc, char * argv[]) {
    
    // process command line option
    static int compress_flag = 0;
    int *portno = NULL;
    char *hostname = NULL, *logFilename = NULL;
    
    
    static struct option long_options[] = {
        {"compress", no_argument, &compress_flag, 1},
        {"log", required_argument, NULL, 'l'},
        {"port", required_argument, NULL, 'p'},
        {"host", required_argument, NULL, 'h'},
        {0, 0, 0, 0} // last element of the array needs be filled this way
    };
    int opt;
    int log_fds = 0;
    while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (opt) {
            case 0: // encountered flag
                break;
            case 'p':
                portno = sys_malloc(sizeof(int), "allocate portno");
                *portno = atoi(optarg);
                break;
            case 'h':
                hostname = malloc(strlen(optarg) + 1);
                strcpy(hostname, optarg);
                break;
            case 'l':
                logFilename = malloc(strlen(optarg) + 1);
                strcpy(logFilename, optarg);
                log_fds = open(logFilename, O_CREAT | O_WRONLY | O_TRUNC | O_SYNC, S_IRWXU);
                if (log_fds < 0) {
                    fprintf(stderr, "cannot write to non-writable file");
                    exit(1);
                }
                break;
            default: // '?'
                error("Usage: ./client --port=# [--host=name] [--shell] [--log] [--compress]\n");
                break;
        }
    }
    if (portno == NULL) {
        fprintf(stderr, "Usage: ./client --port=# [--host=name] [--shell] [--log] [--compress]\n");
        exit(EXIT_FAILURE);
    }
    if (hostname == NULL) {
        hostname = malloc(strlen("localhost") + 1);
        strcpy(hostname, "localhost");
    }

    // termios setup
    struct termios terminal_configure;
    if (tcgetattr(STDIN_FILENO, &terminal_defaults) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    
    terminal_configure = terminal_defaults;
    terminal_configure.c_iflag = ISTRIP; /* only lower 7 bits */
    terminal_configure.c_oflag = 0;      /* no processing     */
    terminal_configure.c_lflag = 0;      /* no processing     */
    
    if (tcsetattr(STDIN_FILENO, TCSANOW, &terminal_configure) == -1) {
        perror("tcsetattr initialize new non-canonical mode");
        exit(EXIT_FAILURE);
    }
    
    atexit(restore_term);
    
    
    /*
    if (shell_flag) {
        fprintf(stdout, "shell flag set! \n");
    }
    if (compress_flag) {
        fprintf(stdout, "compress flag set! \n");
    }
    if (log_flag) {
        fprintf(stdout, "log flag set! \n");
    }
    if (port != -1) {
        fprintf(stdout, "port number is: %d \n", port);
    }
    if (host != NULL) {
        fprintf(stdout, "host is: %s\n", host);
    }
     */
    
    // set up socket
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error("opening socket");
    }
    if ((server = gethostbyname(hostname)) == NULL) {
        error("no such host");
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *) &serv_addr.sin_addr.s_addr, (char*) server -> h_addr_list[0], server -> h_length);
    serv_addr.sin_port = htons(*portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("error connecting");
    }
    
    // buffer setup
    int BUFF_SIZE = 512, message_size = 4096;
    char *buffer = sys_malloc(BUFF_SIZE, "malloc buffer"); /* allocate a 1K buffer */
    char *message = sys_malloc(message_size, "malloc message");
    
    // User Interaction
    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO; // keyboard input
    fds[1].fd = sockfd;       // served data
    fds[0].events = POLLIN | POLLERR | POLLHUP | POLLRDHUP;
    fds[1].events = POLLIN | POLLERR | POLLHUP | POLLRDHUP;
    
    // Log option
    /*
    int log_fds = 0;
    if (logFilename != NULL) {
        log_fds = open(logFilename, O_CREAT | O_WRONLY | O_TRUNC | O_SYNC, S_IRWXU);
        if (log_fds < 0) {
            fprintf(stderr, "cannot write to non-writable file");
            exit(1);
        }
    }*/
    
    // echo user input cr or lf as cr lf, send to server as is
    char c, output[2] = {13, 10}; // cr lf
    int num_c, exit_flag = 0;
    while (1) {
        int p = exit_flag ? poll(&fds[1], 1, 0) : poll(fds, 2, 0);
        if (p > 0) { // there is stuff to process
            memset(buffer, 0, BUFF_SIZE);
            if ((fds[0].revents & POLLIN) && !exit_flag && ((num_c = (int) read(STDIN_FILENO, buffer, BUFF_SIZE)) > 0)) {
                for (int i = 0; i < num_c; i++) {
                    c = buffer[i];
                    if ( c == 4 ) { /* ^D */
                        sys_write(STDOUT_FILENO, "^D", 2, "echo ^D");
                        exit_flag = 1;
                    } else if (c == 3) { /* ^C */
                        sys_write(STDOUT_FILENO, "^C", 2, "echo ^C");
                        exit_flag = 1;
                    } else if (c == 10 || c == 13) {
                        sys_write(STDOUT_FILENO, output, 2, "echo endline");
                    } else {
                        sys_write(STDOUT_FILENO, &c, 1, "echo normal char");
                    } // end if
                } // end for
                if (compress_flag) {
                    num_c = compressMessage(buffer, num_c, message, &message_size);
                    sys_write(sockfd, message, num_c, "write to server as is");
                    if(logFilename != NULL && dprintf(log_fds, "SENT %d bytes: ", num_c) < 0) {
                        error("logging\n");
                    }
                    sys_write(log_fds, message, num_c, "message");
                    sys_write(log_fds, "\n", 1, "newline");
                } else {
                    sys_write(sockfd, buffer, num_c, "write to server as is");
                    if(logFilename != NULL && dprintf(log_fds, "SENT %d bytes: %s\n", num_c, buffer) < 0) {
                        error("logging\n");
                    }
                }
                
            } // end if handling stdin
            // echo server returns cr or lf as cr lf
            memset(buffer, 0, BUFF_SIZE);
            if ((fds[1].revents & POLLIN) && ((num_c = (int) read(fds[1].fd, buffer, BUFF_SIZE)) > 0)) { // have input from server
                if(logFilename != NULL && dprintf(log_fds, "RECEIVED %d bytes: %s\n", num_c, buffer) < 0) {
                    error("logging\n");
                }
                if (compress_flag) {
                    num_c = decompressMessage(buffer, num_c, message, &message_size);
                }
                char *temp = compress_flag ? message : buffer;
                for (int i = 0; i < num_c; i++) {
                    c = temp[i];
                    if (c == 10 || c == 13) {
                        sys_write(STDOUT_FILENO, output, 2, "echo server endline");
                    } else {
                        sys_write(STDOUT_FILENO, &c, 1, "echo server normal char");
                    } //end if
                } // end for
            } // end if handling socket
            if (fds[1].revents & (POLLHUP | POLLERR | POLLRDHUP)) { // socket closed
                sys_close(fds[1].fd, "close socket");
                break;
            } // end if
        } else if (p == 0) {
            continue;
        } else {
            error("poll");
        }
    }
    /*
    if (write(sockfd, "hello world", 12) < 0) {
        error("writing to socket");
    }
    sys_read(sockfd, buffer, BUFF_SIZE, "read from socket");
    fprintf(stdout, "%s", buffer);
    */
    
    // tear down
    free(hostname);
    free(logFilename);
    free(portno);
    if (logFilename != NULL) {
        sys_close(log_fds, "Log file");
    }
    
    // buffer cleanup
    free(buffer);
    free(message);
    if (DEBUG_FLAG) {
        fprintf(stdout, "All done!\n");
    }
}

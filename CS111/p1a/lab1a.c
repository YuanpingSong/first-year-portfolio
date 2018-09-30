//NAME: Yuanping Song
//EMAIL: yuanping.song@outlook.com
//ID:

#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>




struct termios terminal_defaults;

void restore_term() {
    if (tcsetattr(STDIN_FILENO, TCSANOW, &terminal_defaults) == -1) {
        perror("tcsetattr return to canonnical mode");
        exit(EXIT_FAILURE);
    }
}

void sigpipe_handler(int signum) {
    fprintf(stdout, "sigpipe encountered: %d", signum);
    exit(EXIT_SUCCESS);
}

void sys_write(int fd, const void *buf, size_t n, char *msg) {
    if(write(fd, buf, n) < 0) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

void sys_close(int fd, char *msg) {
    if (close(fd) < 0) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

void sys_waitpid(pid_t pid, int *status, int options, char *msg) {
    if(waitpid(pid, status, options) < 0) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char * argv[]) {
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
    
    // process command line option
    static int shell_flag = 0;
    static struct option long_options[] = {
        {"shell", no_argument, &shell_flag, 1},
        {0, 0, 0, 0} // last element of the array needs be filled this way
    };
    int val = getopt_long(argc, argv, "", long_options, NULL);
    if (val == '?') {
        fprintf(stderr, "getopt_long encountered ambiguous command line argument\nusage: ./labla [--shell]\n");
        exit(EXIT_FAILURE);
    }
    
    // buffer setup
    const int BUFF_SIZE = 1024;
    char *buffer = malloc(BUFF_SIZE); /* allocate a 1K buffer */
    if (!buffer) {
        perror("malloc buffer");
        exit(EXIT_FAILURE);
    }
    
    // shell option
    if (shell_flag) {
        // Note: [0] is read and [1] is write
        // send information ...
        int from_parent_to_child[2];
        int from_child_to_parent[2];
        if (pipe(from_parent_to_child) < 0) {
            perror("from parent to child pipe");
            exit(EXIT_FAILURE);
        }
        if (pipe(from_child_to_parent)) {
            perror("from child to parent pipe");
            exit(EXIT_FAILURE);
        }
        
        int rc = fork();
        if (rc < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (rc == 0) { // child process goes here
            if(close(from_child_to_parent[0]) < 0) { // only write
                perror("In Child: from c to p read end");
                exit(EXIT_FAILURE);
            }
            if (close(from_parent_to_child[1]) < 0) { // only read
                perror("In Child: from p to c write end");
                exit(EXIT_FAILURE);
            }
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
            
            // execute shell
            if (execl("/bin/bash", "bash", 0) < 0) {
                perror("In child: exec bash");
                exit(EXIT_FAILURE);
            }
            
        } else { // parent process goes here
            if(close(from_child_to_parent[1]) < 0) { // only read
                perror("In Parent: from c to p write end");
                exit(EXIT_FAILURE);
            }
            if (close(from_parent_to_child[0]) < 0) { // only write
                perror("In Parent: from p to c read end");
                exit(EXIT_FAILURE);
            }
            
            signal(SIGPIPE, sigpipe_handler);
            
            struct pollfd fds[2];
            fds[0].fd = STDIN_FILENO;
            fds[1].fd = from_child_to_parent[0];
            fds[0].events = POLLIN | POLLERR | POLLHUP;
            fds[1].events = POLLIN | POLLERR | POLLHUP;
            
            char c, output[2] = {13, 10}; // cr lf
            int num_c, exit_flag = 0;
            while (1) {
                int p = poll(fds, 2, 0);
                if (p > 0) { // there is stuff to process!
                    if ((fds[0].revents & POLLIN) && !exit_flag) { // have input from stdin and pipe not closed
                        if((num_c = read(STDIN_FILENO, buffer, BUFF_SIZE)) > 0) {
                            for (int i = 0; i < num_c; i++) {
                                c = buffer[i];
                                if ( c == 4 ) { /* ^D */
                                    sys_write(STDOUT_FILENO, "^D", 2, "shell stdout ^D");
                                    sys_close(from_parent_to_child[1], "p2c write");
                                    exit_flag = 1;
                                    break;
                                } else if (c == 3) { /* ^C */
                                    sys_write(STDOUT_FILENO, "^C", 2, "shell stdout ^C");
                                    kill(rc, SIGINT);
                                } else if (c == 10 || c == 13) {
                                    sys_write(STDOUT_FILENO, output, 2, "shell stdout endline");
                                    sys_write(from_parent_to_child[1], &output[1], 1, "shell p2c endline");
                                } else {
                                    sys_write(STDOUT_FILENO, &c, 1, "shell stdout normal char");
                                    sys_write(from_parent_to_child[1], &c, 1, "shell p2c read normal char");
                                } // end if
                            } // end for
                        } // end if
                    }
                    if (fds[1].revents & POLLIN) { // have input from shell
                        if ((num_c = read(fds[1].fd, buffer, BUFF_SIZE)) > 0) {
                            for (int i = 0; i < num_c; i++) {
                                c = buffer[i];
                                if (c == 10 || c == 13) {
                                    sys_write(STDOUT_FILENO, output, 2, "shell stdout endline");
                                } else {
                                    sys_write(STDOUT_FILENO, &c, 1, "shell stdout normal char");
                                }
                            }
                        }
                    }
                    if (fds[1].revents & (POLLHUP | POLLERR)) {
                        sys_close(fds[1].fd, "shell fds[1].fd");
                        int wstatus;
                        sys_waitpid(rc, &wstatus, 0, "parent waiting child");
                        WEXITSTATUS(wstatus);
                        fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(wstatus), WEXITSTATUS(wstatus));
                        break; // stop application
                    }
                } else if (p == 0) { // streams aren't ready
                    continue;
                } else { // error
                    perror("poll");
                    exit(EXIT_FAILURE);
                } // end poll if
            } // end while
        } // end parent
    } else { // non-shell branch
        // process inputs
        int num_c;
        char c;
        int exit_flag = 0;
        while((num_c = read(STDIN_FILENO, buffer, BUFF_SIZE)) > 0) {
            for (int i = 0; i < num_c; i++) {
                c = buffer[i];
                if ( c == 4 ) { /* ^D */
                    exit_flag = 1;
                    break;
                } else if (c == 10 || c == 13) {
                    char output[2] = {13, 10};
                    sys_write(STDOUT_FILENO, output, 2, "non-shell stdout endline");
                } else {
                    sys_write(STDOUT_FILENO, &c, 1, "non-shell stdout normal char");
                } // end if
            } // end for
            if (exit_flag) {
                break;
            }
        } // end while
    }
    
    // buffer cleanup
    free(buffer);
    
    return 0;
}

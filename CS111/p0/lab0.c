// NAME: Yuanping Song
// EMAIL: yuanping.song@outlook.com
// ID:


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

void segfaultHandler(int signum) {
    fprintf(stderr, "Segfault caught, error code: %d, exiting with return code 4\n", signum);
    exit(4);
}

void causeSegfault() {
    char * ptr = NULL;
    *ptr = 'A';
}

int main(int argc, char * argv[]) {
    // we have no single character flags
    const char optstring[] = "";
    static char* input_file = NULL;
    static char* output_file = NULL;
    static int segfault_flag = 0;
    static int catch_flag = 0;
    
    static struct option long_options[] = {
        {"input", required_argument, NULL, 1},
        {"output", required_argument, NULL, 2},
        {"segfault", no_argument, &segfault_flag, 1},
        {"catch", no_argument, &catch_flag, 1},
        {0, 0, 0, 0} // last element of the array needs be filled this way
    };
    
    int c;
    while(1) {
        c = getopt_long(argc, argv, optstring, long_options, NULL);
        if ( c == -1) {
            break;
        } else if( c == 1) {
            input_file = malloc(strlen(optarg) + 1);
            if (input_file == NULL) {
                fprintf(stderr, "Error allocating buffer for input file name using malloc\n%s", strerror(errno));
                _exit(-1);
            }
            strcpy(input_file, optarg);
            continue;
        } else if( c == 2) {
            output_file = malloc(strlen(optarg) + 1);
            if (output_file == NULL) {
                perror("Error allocating buffer for output file name using malloc");
                _exit(-1);
            }
            strcpy(output_file, optarg);
            continue;
        } // end if
        switch (c) {
            case 0:
                // catch, segfault come to this branch
                // do nothing, no error to handle
                break;
            case '?':
                fprintf(stderr, "getopt_long encountered ambiguous command line argument\nusage: ./lab0 [--input=FILE1] [--output=FILE2] [--segfault] [--catch]\n");
                _exit(1);
                break;
            default:
                // should not come here
                fprintf(stderr, "unexpected value returned by getopt_long\n");
                _exit(-1);
                break;
        } // end switch
    } // end while
    // debugging code:
    //printf("input: %s, output: %s, segfault: %d, catch: %d\n", input_file, output_file, segfault_flag, catch_flag);
    
    // file redirection
    if (input_file != NULL) {
        int ifd = open(input_file, O_RDONLY);
        if (ifd == -1) {
            fprintf(stderr, "%s not opened", input_file);
            perror("problem with --input, error opening input file");
            _exit(2);
        }
        close(STDIN_FILENO);
        dup(ifd);
        close(ifd);
    }
    
    if (output_file != NULL) {
        int ofd = open(output_file, O_WRONLY ^ O_CREAT, S_IRWXU);
        if (ofd == -1) {
            fprintf(stderr, "%s not created", output_file);
            perror("problem with --output, error creating output file");
            _exit(3);
        }
        close(STDOUT_FILENO);
        dup(ofd);
        close(ofd);
    }
    
    if (catch_flag) {
        signal(SIGSEGV, &segfaultHandler);
    }
    
    if (segfault_flag) {
        causeSegfault();
    }
    
    
    const int BUFFER_SIZE   = 100; // this constant can be increased to cut down on the number of sys call.
    char buffer[BUFFER_SIZE];
    ssize_t numBytes;
    
    // TODO: implement error handling for read / write
    
    while ( (numBytes = read(STDIN_FILENO, &buffer, BUFFER_SIZE)) > 0) {
        write(STDOUT_FILENO, buffer, numBytes);
    }
    
    free(input_file);
    free(output_file);
    
    _exit(0);
}

//
//  main.c
//  p2a
//
//  Created by Yuanping Song on 8/3/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h> // link with -lrt
#include <pthread.h> // link with -pthread

static int opt_yield = 0;
static int spin_lock = 0;

pthread_mutex_t add_lock = PTHREAD_MUTEX_INITIALIZER;

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield) {
        sched_yield();
    }
    *pointer = sum;
}

void mutex_add(long long *pointer, long long value) {
    pthread_mutex_lock(&add_lock);
    long long sum = *pointer + value;
    if (opt_yield) {
        sched_yield();
    }
    *pointer = sum;
    pthread_mutex_unlock(&add_lock);
}

void spin_add(long long *pointer, long long value) {
    while(__sync_lock_test_and_set(&spin_lock, 1) == 1){
        // spin
    }
    long long sum = *pointer + value;
    if (opt_yield) {
        sched_yield();
    }
    *pointer = sum;
    __sync_lock_release(&spin_lock);
}

void atom_add(long long *pointer, long long value) {
    long long old_val, new_val, actual = *pointer;
    do {
        old_val = actual;
        new_val = old_val + value;
        if (opt_yield) {
            sched_yield();
        }
    } while (old_val != (actual = __sync_val_compare_and_swap(pointer, old_val, new_val)));
}

struct thread_env {
    long long *pointer;
    long long value;
    int iteration_count;
    char sync_flag;
};

void* thread_func(void *arg) {
    struct thread_env* env = (struct thread_env *) arg;
    void (* func_add)(long long*, long long);
    switch (env->sync_flag) {
    case 'm':
        func_add = &mutex_add;
        break;
    case 's':
        func_add = &spin_add;
        break;
    case 'c':
        func_add = &atom_add;
        break;
    default:
        func_add = &add;
        break;
    }
    int i;
    for(i = 0; i < env -> iteration_count; i++) {
        func_add(env -> pointer, env -> value);
    }
    for(i = 0; i < env -> iteration_count; i++) {
        func_add(env -> pointer, -(env -> value));
    }
    pthread_exit(NULL);
}

int main(int argc, char * argv[]) {
    // Initialize Defaults
    int thread_count = 1, iteration_count = 1;
    long long counter = 0;
    char sync_flag = 0;
    
    // process command line option
    static struct option long_options[] = {
        {"threads", required_argument, NULL, 't'},
        {"iterations", required_argument, NULL, 'i'},
        {"sync", required_argument, NULL, 's'},
        {"yield", no_argument, &opt_yield, 1},
        {0, 0, 0, 0} // last element of the array needs be filled this way
    };
    int opt;
    while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (opt) {
            case 't':
                thread_count = atoi(optarg);
                break;
            case 'i':
                iteration_count = atoi(optarg);
                break;
            case 's':
                sync_flag = *optarg;
                if (strlen(optarg) != 1 || (sync_flag != 'm' && sync_flag != 's' && sync_flag != 'c') ) {
                    fprintf(stderr, "usage: --sync=m|s|c\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 0:
                break;
            default: // '?'
                fprintf(stderr, "Usage: ./lab2_add [--threads=1] [--iterations=1] [--sync=m|s|c] [--yield]\n");
                exit(EXIT_FAILURE);
                break;
        } // end switch
    } // end while
    if (thread_count < 1) {
        fprintf(stderr, "Thread count is at least 1!\n");
        exit(EXIT_FAILURE);
    }
    if (iteration_count < 1) {
        fprintf(stderr, "Iteration count is at least 1!\n");
        exit(EXIT_FAILURE);
    }
    
    // Get start time
    struct timespec starting_time;
    if (clock_gettime(CLOCK_REALTIME, &starting_time) == -1) {
        perror("lab2_add");
        exit(EXIT_FAILURE);
    }
    
    // Set up threads
    struct thread_env env;
    env.iteration_count = iteration_count;
    env.pointer = &counter;
    env.value = 1;
    env.sync_flag = sync_flag;
    pthread_t *thread = malloc(thread_count * sizeof(pthread_t));
    
    
    int i;
    for (i = 0 ; i < thread_count; i++) {
        if (pthread_create(&thread[i], NULL, thread_func, &env) != 0) {
            perror("lab2_add");
            exit(EXIT_FAILURE);
        }
    }
    
    // wait for threads to join.
    for (i = 0; i < thread_count; i++) {
        if (pthread_join(thread[i], NULL) != 0) {
            perror("lab2_add");
            exit(EXIT_FAILURE);
        }
    }
    
    // Get end time
    struct timespec end_time;
    if (clock_gettime(CLOCK_REALTIME, &end_time) == -1) {
        perror("lab2_add");
        exit(EXIT_FAILURE);
    }
    
    // Calc output
    char name_root[] = "add";
    char name_yield[] = "-yield";
    char name_no_yield[] = "";
    char name_m_sync[] = "-m";
    char name_s_sync[] = "-s";
    char name_c_sync[] = "-c";
    char name_no_sync[] = "-none";

    
    char* has_yield = opt_yield ? name_yield : name_no_yield;
    char* which_sync;
    switch (sync_flag) {
        case 'm':
            which_sync = name_m_sync;
            break;
        case 'c':
            which_sync = name_c_sync;
            break;
        case 's':
            which_sync = name_s_sync;
            break;
        default:
           which_sync = name_no_sync;
            break;
    }
   
    long long ops =  thread_count * iteration_count * 2;
    long long total_time = (long long)(end_time.tv_sec - starting_time.tv_sec) * 1000000000 +  end_time.tv_nsec - starting_time.tv_nsec;
    long long avg_time = total_time / ops;
    
    fprintf(stdout, "%s%s%s,%d,%d,%lld,%lld,%lld,%lld\n", name_root, has_yield, which_sync, thread_count, iteration_count, ops, total_time, avg_time, counter);
    // tear-down
    free(thread);
    return 0;
}

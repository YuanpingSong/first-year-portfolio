//
//  lab2_list.c
//  p2a
//
//  Created by Yuanping Song on 8/4/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include "SortedList.h"

SortedListElement_t *pool = NULL;
struct thread_env* env = NULL;
pthread_t *thread = NULL;
int opt_yield = 0;
pthread_mutex_t list_lock = PTHREAD_MUTEX_INITIALIZER;
int spin_lock = 0;

struct thread_env {
    SortedList_t *list; // shared list
    SortedList_t *pool; // partition of pool
    int iteration_count;
};

void* thread_func(void *arg) {
    //fprintf(stdout, "regular func called!\n");
    struct thread_env* env = (struct thread_env *) arg;
    int i, len;
    for (i = 0; i < env->iteration_count; i++) {
        SortedList_insert(env -> list, (env->pool) + i);
    }
    len = SortedList_length(env->list);
    if (len == -1) {
        fprintf(stderr, "inconsistency Detected! A\n");
        exit(2);
    }
    for (i = 0; i < env->iteration_count; i++) {
        if(SortedList_delete((env->pool) + i) != 0) {
            fprintf(stderr, "inconsistency Detected! B\n");
            exit(2);
        }
    }
    pthread_exit(NULL);
}

void* mutex_thread_func(void *arg) {
    //fprintf(stdout, "mutex func called!\n");
    struct thread_env* env = (struct thread_env *) arg;
    int i, len;
    for (i = 0; i < env->iteration_count; i++) {
        pthread_mutex_lock(&list_lock);
        SortedList_insert(env -> list, (env->pool) + i);
        pthread_mutex_unlock(&list_lock);
    }
    pthread_mutex_lock(&list_lock);
    len = SortedList_length(env->list);
    pthread_mutex_unlock(&list_lock);
    if (len == -1) {
        fprintf(stderr, "inconsistency Detected! C\n");
        exit(2);
    }
    for (i = 0; i < env->iteration_count; i++) {
        pthread_mutex_lock(&list_lock);
        if(SortedList_delete((env->pool) + i) != 0) {
            fprintf(stderr, "inconsistency Detected! D\n");
            exit(2);
        }
        pthread_mutex_unlock(&list_lock);
    }
    pthread_exit(NULL);
}

void* spin_thread_func(void *arg) {
    //fprintf(stdout, "spin func called!\n");
    struct thread_env* env = (struct thread_env *) arg;
    int i, len;
    for (i = 0; i < env->iteration_count; i++) {
        while(__sync_lock_test_and_set(&spin_lock, 1) == 1){
            // spin
        }
        SortedList_insert(env -> list, (env->pool) + i);
        __sync_lock_release(&spin_lock);
    }
    while(__sync_lock_test_and_set(&spin_lock, 1) == 1){
        // spin
    }
    len = SortedList_length(env->list);
    __sync_lock_release(&spin_lock);
    if (len == -1) {
        fprintf(stderr, "inconsistency Detected! E\n");
        exit(2);
    }
    for (i = 0; i < env->iteration_count; i++) {
        while(__sync_lock_test_and_set(&spin_lock, 1) == 1){
            // spin
        }
        if(SortedList_delete((env->pool) + i) != 0) {
            fprintf(stderr, "inconsistency Detected! F\n");
            exit(2);
        }
        __sync_lock_release(&spin_lock);
    }
    pthread_exit(NULL);
}

void clean_up() {
    free(pool);
    free(thread);
    free(env);
}

void sigsegv_handler(int signum) {
    fprintf(stderr, "Segfault encountered! signum is: %d\n", signum);
    exit(2);
}

int main(int argc, char * argv[]) {
    // Initialize Defaults
    int thread_count = 1, iteration_count = 1;
    char sync_flag = 0; // a character representing the type of sync mechanism employed, default null char
    
    // register handlers
    atexit(clean_up);
    signal(SIGSEGV, sigsegv_handler);
    
    // process command line option
    static struct option long_options[] = {
        {"threads", required_argument, NULL, 't'},
        {"iterations", required_argument, NULL, 'i'},
        {"sync", required_argument, NULL, 's'},
        {"yield", required_argument, NULL, 'y'},
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
            case 'y':
                if (strchr(optarg, 'i') != NULL) {
                    opt_yield |= INSERT_YIELD;
                }
                if (strchr(optarg, 'd') != NULL) {
                    opt_yield |= DELETE_YIELD;
                }
                if (strchr(optarg, 'l') != NULL) {
                    opt_yield |= LOOKUP_YIELD;
                }
                if (opt_yield == 0) {
                    fprintf(stderr, "usage: --yield=[idl]]");
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                sync_flag = *optarg;
                if (strlen(optarg) != 1 || (sync_flag != 'm' && sync_flag != 's') ) {
                    fprintf(stderr, "usage: --sync=m|s\n");
                    exit(EXIT_FAILURE);
                }
                break;
            default: // '?'
                fprintf(stderr, "Usage: ./lab2_list [--threads=1] [--iterations=1] [--yield=#] [--sync=m|s]\n");
                exit(EXIT_FAILURE);
                break;
        } // end switch
    } // end while
    
    /*if (opt_yield & INSERT_YIELD) {
        fprintf(stdout, "Input flag set!\n");
    }
    if (opt_yield & DELETE_YIELD) {
        fprintf(stdout, "Delete flag set!\n");
    }
    if (opt_yield & LOOKUP_YIELD) {
        fprintf(stdout, "Lookup flag set!\n");
    }
    if (sync_flag == 'm') {
        fprintf(stdout, "Mutex flag set!\n");
    }
    if (sync_flag == 's') {
        fprintf(stdout, "Spin flag set!\n");
    }*/
    
    // Initialize List
    SortedList_t *lt = malloc(sizeof(SortedList_t));
    if (lt == NULL) {
        perror("lab2_list");
        exit(EXIT_FAILURE);
    }
    
    lt->key = NULL;
    lt->prev = lt;
    lt->next = lt;
    
    // seed random number generator
    srand((unsigned)time(NULL));
    
    // generate random list elements
    long long num_elements = thread_count * iteration_count;
    pool = malloc(sizeof(SortedListElement_t) * num_elements);
    if (pool == NULL) {
        perror("lab2_list");
        exit(EXIT_FAILURE);
    }
    int i, j;
    char c;
    for(i = 0; i < num_elements; i++) { // initialize elements
        int size = 1 + rand() % 20; // a number from 1 to 20;
        //fprintf(stdout,"size is %ld\n", size);
        char *key = malloc(size + 1);
        if (key == NULL) {
            perror("lab2_list");
            exit(EXIT_FAILURE);
        }
        pool[i].prev = NULL;
        pool[i].next = NULL;
        pool[i].key = key;
        for (j = 0; j < size; j++) { // generate key
            c = 'a' + rand() % 26;
            key[j] = c;
        }
        key[size] = 0; // null byte
        //fprintf(stdout,"Key is %s\n", key);
    }
    
    /*for (i = 0; i < num_elements; i++) {
        fprintf(stdout, "prev is %p, next is %p, key is %s\n", pool[i].prev, pool[i].next, pool[i].key);
    }*/
    
    // thread creation
    env = malloc(thread_count * sizeof(struct thread_env));
    if (env == NULL) {
        perror("lab2_list");
        exit(EXIT_FAILURE);
    }
    thread = malloc(thread_count * sizeof(pthread_t));
    if (thread == NULL) {
        perror("lab2_list");
        exit(EXIT_FAILURE);
    }
    void* (* func_ptr) (void*);
    switch (sync_flag) {
        case 'm':
            func_ptr = mutex_thread_func;
            break;
        case 's':
            func_ptr = spin_thread_func;
            break;
        default:
            func_ptr = thread_func;
            break;
    }
    
    for (i = 0; i < thread_count; i++) {
        env[i].list = lt;
        env[i].iteration_count = iteration_count;
        env[i].pool = pool + (i * iteration_count);
    }
    
    // Get start time
    struct timespec starting_time;
    if (clock_gettime(CLOCK_REALTIME, &starting_time) == -1) {
        perror("lab2_list");
        exit(EXIT_FAILURE);
    }
    
    for (i = 0 ; i < thread_count; i++) {
        if (pthread_create(&thread[i], NULL, func_ptr, &env[i]) != 0) {
            perror("lab2_list");
            exit(EXIT_FAILURE);
        }
    }
    
    // wait for threads to join.
    for (i = 0; i < thread_count; i++) {
        if (pthread_join(thread[i], NULL) != 0) {
            perror("lab2_list");
            exit(EXIT_FAILURE);
        }
    }
    
    // Get end time
    struct timespec end_time;
    if (clock_gettime(CLOCK_REALTIME, &end_time) == -1) {
        perror("lab2_list");
        exit(EXIT_FAILURE);
    }
    
    if(SortedList_length(lt) != 0){
        fprintf(stderr, "Inconsistent Result! G \n");
        exit(2);
    }
    
    // Calc output
    char name_root[] = "list";
    char yield_opts[5] = "-", *name_yield;
    char name_no_sync[] = "-none";
    char name_no_yield[] = "-none";
    if (sync_flag != 0) {
        name_no_sync[1] = sync_flag;
        name_no_sync[2] = 0;
     }
    
    if (opt_yield & INSERT_YIELD) {
        strcat(yield_opts, "i");
    }
    if (opt_yield & DELETE_YIELD) {
        strcat(yield_opts, "d");
    }
    if (opt_yield & LOOKUP_YIELD) {
        strcat(yield_opts, "l");
    }
    name_yield = (strlen(yield_opts) > 1) ? yield_opts : name_no_yield;
    
    
    long long ops =  thread_count * iteration_count * 3;
    long long total_time = (long long)(end_time.tv_sec - starting_time.tv_sec) * 1000000000 +  end_time.tv_nsec - starting_time.tv_nsec;
    long long avg_time = total_time / ops;
    
    fprintf(stdout, "%s%s%s,%d,%d,%d,%lld,%lld,%lld\n", name_root, name_yield, name_no_sync, thread_count, iteration_count,1, ops, total_time, avg_time);
    exit(0);
}




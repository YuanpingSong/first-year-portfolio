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
#include <gperftools/profiler.h>

SortedListElement_t *pool = NULL; // list elements to be manipulated
struct thread_env* env = NULL; // thread env
pthread_t *thread = NULL; // arry of pids
int opt_yield = 0, num_list = 1;

SortedList_t *lists = NULL; // sublists
int *spin_locks = NULL; // array of spin locks, one for each list
pthread_mutex_t *mutexes = NULL; //array of mutexes, one for each list
long long num_elements = 0; // number of elements in pool



int hash(const char *string) {
	int i = 0, counter = 0;
	while (string && string[i] != 0) {
		counter += string[i];
		i++;
	}
	return counter;
}


/* Subtract the ‘struct timeval’ values X and Y,
 storing the result in RESULT.
 Return 1 if the difference is negative, otherwise 0. */

int
timespec_subtract (struct timespec *result, struct timespec *x, struct timespec *y)
{
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_nsec < y->tv_nsec) {
		long nsec = (long)(y->tv_nsec - x->tv_nsec) / 1000000000 + 1;
		y->tv_nsec -= 1000000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_nsec - y->tv_nsec > 1000000000) {
		long nsec = (long) (x->tv_nsec - y->tv_nsec) / 1000000000;
		y->tv_nsec += 1000000000 * nsec;
		y->tv_sec -= nsec;
	}
	
	/* Compute the time remaining to wait.
	 tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_nsec = x->tv_nsec - y->tv_nsec;
	
	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

void
timespec_add(struct timespec *result, struct timespec *x, struct timespec *y) {
	result -> tv_sec = (x -> tv_sec + y -> tv_sec) + (x -> tv_nsec + y -> tv_nsec) / 1000000000;
	result -> tv_nsec = (x -> tv_nsec + y -> tv_nsec) % 1000000000;
}

struct thread_env {
    SortedList_t *pool; // partition of pool
    int iteration_count;
	struct timespec accumulator;
};

void* thread_func(void *arg) {
    //fprintf(stdout, "regular func called!\n");
    struct thread_env* env = (struct thread_env *) arg;
    int i, len = 0, list;
    for (i = 0; i < env->iteration_count; i++) {
		list = hash((env -> pool + i) ->key) % num_list;
        SortedList_insert(&lists[list], (env->pool) + i);
    }
	for (int i = 0; i < num_list; i++) {
		int temp = SortedList_length(&lists[i]);
		if (temp == -1) {
			fprintf(stderr, "inconsistency Detected! A\n");
			exit(2);
		}
		len += temp;
	}
	
    for (i = 0; i < env->iteration_count; i++) {
        if(SortedList_delete((env->pool) + i) != 0) {
            fprintf(stderr, "inconsistency Detected! B\n");
            exit(2);
        }
    }
    pthread_exit(NULL);
}

void mutex_lock_and_accumulate(pthread_mutex_t *mutex, struct timespec *accumulator) {
	struct timespec start_time, end_time, diff;
	if (clock_gettime(CLOCK_REALTIME, &start_time) == -1) {
		perror("lab2_list");
		exit(EXIT_FAILURE);
	}
	pthread_mutex_lock(mutex);
	if (clock_gettime(CLOCK_REALTIME, &end_time) == -1) {
		perror("lab2_list");
		exit(EXIT_FAILURE);
	}
	if(timespec_subtract(&diff, &end_time, &start_time) != 0) {
		fprintf(stderr, "Negative difference between end time and start time");
		exit(EXIT_FAILURE);
	}
	
	timespec_add(accumulator, accumulator, &diff);
}

void spin_lock_and_accumulate(int *spin_lock, struct timespec *accumulator) {
	struct timespec start_time, end_time, diff;
	if (clock_gettime(CLOCK_REALTIME, &start_time) == -1) {
		perror("lab2_list");
		exit(EXIT_FAILURE);
	}
	while(__sync_lock_test_and_set(spin_lock, 1) == 1){
		// spin
	}
	if (clock_gettime(CLOCK_REALTIME, &end_time) == -1) {
		perror("lab2_list");
		exit(EXIT_FAILURE);
	}
	if(timespec_subtract(&diff, &end_time, &start_time) != 0) {
		fprintf(stderr, "Negative difference between end time and start time");
		exit(EXIT_FAILURE);
	}
	
	timespec_add(accumulator, accumulator, &diff);
}

void* mutex_thread_func(void *arg) {
	
    //fprintf(stdout, "mutex func called!\n");
    struct thread_env* env = (struct thread_env *) arg;
    int i, len = 0, list;
    for (i = 0; i < env->iteration_count; i++) {
		list = hash((env -> pool + i) ->key) % num_list;
		mutex_lock_and_accumulate(&mutexes[list], &(env->accumulator));
        SortedList_insert(&lists[list], (env->pool) + i);
        pthread_mutex_unlock(&mutexes[list]);
	
    }
	for (i = 0; i < num_list; i++) {
		mutex_lock_and_accumulate(&mutexes[i], &(env->accumulator));
		int temp = SortedList_length(&lists[i]);
		if (temp == -1) {
			fprintf(stderr, "inconsistency Detected! C\n");
			exit(2);
		}
		len += temp;
	}
	for (i = 0; i < num_list; i++) {
		pthread_mutex_unlock(&mutexes[i]);
	}
    for (i = 0; i < env->iteration_count; i++) {
		list = hash((env -> pool + i) ->key) % num_list;
        mutex_lock_and_accumulate(&mutexes[list], &(env->accumulator));
        if(SortedList_delete((env->pool) + i) != 0) {
            fprintf(stderr, "inconsistency Detected! D\n");
            exit(2);
        }
        pthread_mutex_unlock(&mutexes[list]);
    }
    pthread_exit(NULL);
}

void* spin_thread_func(void *arg) {
    //fprintf(stdout, "spin func called!\n");
    struct thread_env* env = (struct thread_env *) arg;
    int i, len = 0, list;
    for (i = 0; i < env->iteration_count; i++) {
		list = hash((env -> pool + i) ->key) % num_list;
		spin_lock_and_accumulate(&spin_locks[list], &(env->accumulator));
        SortedList_insert(&lists[list], (env->pool) + i);
        __sync_lock_release(&spin_locks[list]);
    }
	for (i = 0; i < num_list; i++) {
		spin_lock_and_accumulate(&spin_locks[i], &(env->accumulator));
		int temp = SortedList_length(&lists[i]);
		if (temp == -1) {
			fprintf(stderr, "inconsistency Detected! C\n");
			exit(2);
		}
		len += temp;
	}
	
	for (i = 0; i < num_list; i++) {
		__sync_lock_release(&spin_locks[i]);
	}
	
    for (i = 0; i < env->iteration_count; i++) {
		list = hash((env -> pool + i) ->key) % num_list;
        spin_lock_and_accumulate(&spin_locks[list], &(env->accumulator));
        if(SortedList_delete((env->pool) + i) != 0) {
            fprintf(stderr, "inconsistency Detected! F\n");
            exit(2);
        }
        __sync_lock_release(&spin_locks[list]);
    }
    pthread_exit(NULL);
}

void clean_up() {
	int i;
    if (pool) {
        for (i = 0; i < num_elements; i++) {
            free((void *)pool[i].key);
        }
    }
    free(pool);
    free(thread);
    free(env);
	free(lists);
	free(mutexes);
	free(spin_locks);
}

void sigsegv_handler(int signum) {
    fprintf(stderr, "Segfault encountered! signum is: %d\n", signum);
    exit(2);
}

int main(int argc, char * argv[]) {
	
	
    // Initialize Defaults
    int thread_count = 1, iteration_count = 1;
    char sync_flag = 0; // a character representing the type of sync mechanism employed, default null char
	static int profile_flag = 0;
    
    // register handlers
    atexit(clean_up);
    signal(SIGSEGV, sigsegv_handler);
    
    // process command line option
    static struct option long_options[] = {
        {"threads", required_argument, NULL, 't'},
        {"iterations", required_argument, NULL, 'i'},
		{"lists", required_argument, NULL, 'l'},
        {"sync", required_argument, NULL, 's'},
        {"yield", required_argument, NULL, 'y'},
		{"profile", no_argument, &profile_flag, 1},
        {0, 0, 0, 0} // last element of the array needs be filled this way
    };
    int opt;
    while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (opt) {
			case 0:
				break;
            case 't':
                thread_count = atoi(optarg);
				if (thread_count < 1) {
					fprintf(stderr, "thread count must be positive!\n");
					exit(EXIT_FAILURE);
				}
                break;
            case 'i':
                iteration_count = atoi(optarg);
				if (iteration_count < 1) {
					fprintf(stderr, "iteration count must be positive!\n");
					exit(EXIT_FAILURE);
				}
                break;
			case 'l':
				num_list = atoi(optarg);
				if (num_list < 1) {
					fprintf(stderr, "lists must be positive!\n");
					exit(EXIT_FAILURE);
				}
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
                    fprintf(stderr, "usage: --yield=[idl]]\n");
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
				fprintf(stderr, "Usage: ./lab2_list [--threads=1] [--iterations=1] [--yield=#] [--lists=1] [--profile] [--sync=m|s]\n");
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
	
	// Profiler
	if (profile_flag) {
		ProfilerStart("profile.out");
	}
	int i, j;
	char c;
	lists = malloc(sizeof(SortedList_t) * num_list); // array of list heads
	if (lists == NULL) {
		perror("lab2_list");
		exit(EXIT_FAILURE);
	}
	spin_locks = malloc(sizeof(int) * num_list); // array of spin locks
	if (spin_locks == NULL) {
		perror("lab2_list");
		exit(EXIT_FAILURE);
	}
	mutexes = malloc(sizeof(pthread_mutex_t) * num_list); // array of mutexes
	if (mutexes == NULL) {
		perror("lab2_list");
		exit(EXIT_FAILURE);
	}
	
	for (i = 0; i < num_list; i++) {
		lists[i].key = NULL;
		lists[i].prev = &(lists[i]);
		lists[i].next = &(lists[i]);
		spin_locks[i] = 0; // not held
		if(pthread_mutex_init(&mutexes[i], NULL)) {
			perror("lab2_list");
			exit(EXIT_FAILURE);
		}
	}

    // seed random number generator
    srand((unsigned)time(NULL));
    
    // generate random list elements
    num_elements = thread_count * iteration_count;
    pool = malloc(sizeof(SortedListElement_t) * num_elements);
    if (pool == NULL) {
        perror("lab2_list");
        exit(EXIT_FAILURE);
    }
	
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
        env[i].iteration_count = iteration_count;
        env[i].pool = pool + (i * iteration_count);
		env[i].accumulator.tv_sec = 0;
		env[i].accumulator.tv_nsec = 0;
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
	
	// check consistency
	for (i = 0; i < num_list; i++) {
		int temp = SortedList_length(&lists[i]);
		if (temp != 0) {
			fprintf(stderr, "Inconsistent Result! G \n");
			exit(2);
		}
	}
	
	// compute total wait time
	struct timespec total;
	total.tv_nsec = 0;
	total.tv_sec = 0;
	
	for (i = 0; i < thread_count; i++) {
		timespec_add(&total, &(env[i].accumulator), &total);
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
	long long total_wait = total.tv_sec * 1000000000 + total.tv_nsec;
	long long avg_lock = total_wait / ops;
    
    fprintf(stdout, "%s%s%s,%d,%d,%d,%lld,%lld,%lld,%lld\n", name_root, name_yield, name_no_sync, thread_count, iteration_count,num_list, ops, total_time, avg_time, avg_lock);
	
	// Profiler
	if (profile_flag) {
		ProfilerStop();
	}
    exit(0);
}




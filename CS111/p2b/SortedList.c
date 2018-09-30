//
//  lab2_list.c
//  p2b
//
//  Created by Yuanping Song on 8/4/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include "SortedList.h"
#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sched.h>

// in an empty list head -> prev == head && head -> next == head
void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
    SortedListElement_t *prev = list, *next = prev -> next;
    while (next -> key && (strcmp(next -> key, element -> key) < 0)) {
        prev = next;
        next = next -> next;
    }
    element -> prev = prev;
    element -> next = next;
    if (opt_yield && INSERT_YIELD) {
        if(sched_yield() != 0) {
            perror("SortedList.c");
            exit(EXIT_FAILURE);
        }
    }
    prev -> next = element; // critical section
    next -> prev = element; // critical section
}


int SortedList_delete( SortedListElement_t *element) {
    SortedListElement_t *prev = element -> prev, *next = element -> next;
    if (prev -> next != element || next -> prev != element) {
        return 1;
    }
    if (opt_yield & DELETE_YIELD) {
        if(pthread_yield() != 0) {
            perror("SortedList.c");
            exit(EXIT_FAILURE);
        }
    }
    prev -> next = next; // critical section
    next -> prev = prev; // critical section
    return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
    SortedListElement_t *it = list -> next;
    while (it != list) {
        if (strcmp(it -> key, key) == 0) {
            return it;
        } else if (strcmp(it -> key, key) < 0) {
            return NULL; // list is increasing
        }
        if (opt_yield & LOOKUP_YIELD) {
            if(pthread_yield() != 0) {
                perror("SortedList.c");
                exit(EXIT_FAILURE);
            }
        }
        it = it -> next; // critical section
    }
    return NULL;
}

int SortedList_length(SortedList_t *list) {
    int size = 0;
    SortedListElement_t *it = list -> next;
    while (it != list) {
        if (it -> prev -> next != it || it -> next -> prev != it) {
            return -1;
        }
        size++;
        if (opt_yield & LOOKUP_YIELD) {
            if(pthread_yield() != 0) {
                perror("SortedList.c");
                exit(EXIT_FAILURE);
            }
        }
        it = it -> next; // critical section
    }
    return size;
}

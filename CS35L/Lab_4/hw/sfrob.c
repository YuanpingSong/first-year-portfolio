//
//  main.c
//  cs35l_hw4
//
//  Created by Yuanping Song on 5/6/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

void printFrobString(const char *a) {
    int i = 0;
    while (a[i] != ' ') {
        printf("%c",a[i]);
        i++;
    }
    printf("%c", ' ');
}
int frobcmp(char const *a, char const *b) {
    unsigned char decoder = 42;
    unsigned char terminator = ' ' ;
    int i;
    /* compare corresponding bytes */
    for (i = 0; a[i] != terminator && b[i] != terminator; i++) {
        if ((a[i] ^ decoder) < (b[i] ^ decoder) ) {
            return -1;
        }
        if ( (a[i] ^ decoder) > (b[i] ^ decoder) ) {
            return 1;
        }
    }
    /* a is prefix of b */
    if (a[i] == terminator && b[i] != terminator) {
        return -1;
    }
    /* b is prefix of a */
    if (b[i] == terminator && a[i] != terminator) {
        return 1;
    }
    
    /* a and b are completely equal */
    return 0;
}

int ptrcomp(const void *a, const void *b) {
    return frobcmp(*(const char**)a, *(const char**)b);
}

int getLength(const char *a) {
    int i = 0;
    for (; a[i] != ' '; i++) {
        
    }
    return i;
}



int main(int argc, const char * argv[]) {
    
    /* dynamically allocate array of pointers to strings */
    size_t arr_size = 100;
    size_t stringCount = 0;
    char **strings = malloc(arr_size * sizeof(char*));
    if (strings == NULL) {
        fprintf(stderr, "Error allocating a strings buffer of size  %zu",
                arr_size * sizeof(char*));
    }
    
    /* dynamically allocate buffer for input */
    size_t buffer_size = 4096;
    char *buffer = malloc(buffer_size);
    if (buffer == NULL) {
        fprintf(stderr, "Error allocating a input buffer of size  %zu",
                buffer_size);
    }
    
    ssize_t linelen = 0;
    
    while ((linelen = getdelim(&buffer, &buffer_size, ' ', stdin)) > 0) {
        /* input error message */
        if (ferror(stdin)) {
            fprintf(stderr, "Error reading from stdin");
        }
        /* do not read empty line */
        if (strcmp(buffer, "") == 0) {
            continue;
        }
        /* append space if not already */
        if (buffer[linelen - 1] != ' ') {
            buffer[linelen] = ' ';
            linelen++;
        }
        
        strings[stringCount] = malloc(linelen); /* do not copy null char */
        memcpy(strings[stringCount], buffer, linelen);
        stringCount++;
        
        /* dynamically grow strings */
        if (stringCount == arr_size) {
            arr_size *= 2;
            strings = realloc(strings, arr_size);
        }
    }
    /*
    for (int i = 0; i < stringCount; i++) {
        
        printf("%s\n", strings[i]);
    }*/
    
    qsort(strings, stringCount, sizeof(char*), ptrcomp);
    
    for (int i = 0 ; i < stringCount; i++) {
        /* output error message */
        if (ferror(stdout)) {
            fprintf(stderr, "Error printing to stdout");
        }
        printFrobString(strings[i]);
        free(strings[i]);
        printf("\n");
    }
    free(buffer);
    free(strings);
}

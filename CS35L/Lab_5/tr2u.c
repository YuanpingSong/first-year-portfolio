//
//  tr2u.c
//  CS_35L_Assignment_5
//
//  Created by Yuanping Song on 5/11/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

bool isSet(const char operand[]) {
    size_t length = strlen(operand);
    for(int i = 0; i < length; i++) {
        for (int j = i + 1; j < length; j++) {
            if (operand[i] == operand[j]) {
                return false;
            }
        }
    }
    return true;
}

int main(int argc, const char * argv[]) {
    /* Diagonose Operand Error */
    if (argc != 3) {
        fprintf(stderr, "Please enter two operands: <from> and <to>, after program name.\n");
        return 1;
    } else if (strlen(argv[1]) != strlen(argv[2])) {
        fprintf(stderr, "Please enter operands <from> and <to> of the same length.\n");
        return 1;
    } else if (! (isSet(argv[1]) && isSet(argv[2])) ) {
        fprintf(stderr, "<from> and <to> must have all unique characters.\n");
        return 1;
    }
   
    const char* from = argv[1];
    const char* to = argv[2];
    void* buffer = malloc(1); /* char buffer */
    while( read(STDIN_FILENO, buffer, 1) == 1) {
        char * ptr = NULL;
        if ( (ptr = strchr(from, *(const char*) buffer)) != NULL) {
            write(STDOUT_FILENO, &to[ptr - from], 1); /* translliterate */
        } else {
            write(STDOUT_FILENO, buffer, 1); /* No translation */
        }
    }
    free(buffer);
    return 0;
}

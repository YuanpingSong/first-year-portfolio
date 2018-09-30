//
//  main.c
//  CS_35L_Assignment_5
//
//  Created by Yuanping Song on 5/11/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//


/*
 For this laboratory, you will implement transliteration programs tr2b and tr2u that use buffered and unbuffered I/O respectively, and compare the resulting implementations and performance. Each implementation should be a main program that takes two operands from and to that are byte strings of the same length, and that copies standard input to standard output, transliterating every byte in from to the corresponding byte in to. Your implementations should report an error from and to are not the same length, or if from has duplicate bytes. To summarize, your implementations should like the standard utility tr does, except that they have no options, characters like [, - and \ have no special meaning in the operands, operand errors must be diagnosed, and your implementations act on bytes rather than on (possibly multibyte) characters.
 1. Write a C transliteration program tr2b.c that uses getchar and putchar to transliterate bytes as described above.
 2. Write a C program tr2u.c that uses read and write to transliterate bytes, instead of using getchar and putchar. The nbyte arguments to read and write should be 1, so that the program reads and writes single bytes at a time.
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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
    char c = 0;
    const char* from = argv[1];
    const char* to = argv[2];
    while( (c = getchar()) != EOF) {
        char * ptr = NULL;
        if ( (ptr = strchr(from, c)) != NULL) {
            putchar(to[ptr - from]); /* translliterate */
        } else {
            putchar(c); /* No translation */
        }
    }

    return 0;
}

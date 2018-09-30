//
//  tester.c
//  cs35l_hw4
//
//  Created by Yuanping Song on 5/6/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include <stdio.h>
#include <sys/types.h>


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

int main() {
    printf("%d", frobcmp("*~BO", "*hXE]D"));
}

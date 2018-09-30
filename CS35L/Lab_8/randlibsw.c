//
//  randlibsw.c
//  CS35L_Lab8
//
//  Created by Yuanping Song on 5/31/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include "randlib.h"
#include <stdio.h>
#include <stdlib.h>

/* Software implementation.  */
void init (void) __attribute__((constructor));
void fini (void) __attribute__((destructor));

/* Input stream containing random bytes.  */
static FILE *urandstream;

/* Initialize the software rand64 implementation.  */
void
init (void)
{
    urandstream = fopen ("/dev/urandom", "r");
    if (! urandstream)
        abort ();
}

/* Return a random value, using software operations.  */
unsigned long long
rand64 (void)
{
    unsigned long long int x;
    if (fread (&x, sizeof x, 1, urandstream) != 1)
        abort ();
    return x;
}

/* Finalize the software rand64 implementation.  */
void
fini (void)
{
    fclose (urandstream);
}

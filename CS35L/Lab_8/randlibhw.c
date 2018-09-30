//
//  randlibhw.c
//  CS35L_Lab8
//
//  Created by Yuanping Song on 5/31/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include "randlib.h"
#include <immintrin.h>

/* Initialize the hardware rand64 implementation.  */
void
init (void)
{
}

/* Return a random value, using hardware operations.  */
unsigned long long
rand64 (void)
{
    unsigned long long int x;
    while (! _rdrand64_step (&x))
        continue;
    return x;
}

/* Finalize the hardware rand64 implementation.  */
void
fini (void)
{
}


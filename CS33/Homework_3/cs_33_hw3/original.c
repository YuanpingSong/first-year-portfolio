//
//  main.c
//  cs_33_hw3
//
//  Created by Yuanping Song on 5/25/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include <stdio.h>
#include <mm_malloc.h>

void transpose(int *dst, int *src, int dim) {
    int i, j;
    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim; j++) {
            dst[j*dim + i] = src[i * dim + j];
        }
    }
}


#define N 5000

int main() {
    int *src = _mm_malloc(N*N*sizeof(int), 64);
    /*for (int i = 0; i < N * N ; i++) {
        src[i] = i;
    }*/
    int *dest = _mm_malloc(N*N*sizeof(int), 64);
    transpose(dest, src, N);
    /*for (int i = 0; i < N * N; i++) {
        if (i % N == 0 && i != 0)  {
            printf("\n");
        }
        printf("%d ", dest[i]);
        
    }*/
    //transpose_block(src, dest, N, N, lda, lda, 16);
    return 0;
}

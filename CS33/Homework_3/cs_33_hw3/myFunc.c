//
//  myFunc.c
//  cs_33_hw3
//
//  Created by Yuanping Song on 5/26/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include <stdio.h>
#include <mm_malloc.h>


void transpose(int *dest, int *src, int dim) {
    int i,j;
    int row_offset, dest_offset;
    // Perform transposition on 2 by 2 submatrices to take advantage of cache.
    for (i = 0; i + 1 < dim; i += 2) {
        row_offset = i * dim;
        for (j = 0; j + 1 < dim; j += 2) {
            dest_offset = j * dim;
            dest[dest_offset + i] = src[row_offset + j];
            dest[dest_offset + i + 1] = src[row_offset + j + dim];
            dest[dest_offset + i + dim] = src[row_offset + j + 1];
            dest[dest_offset + i + dim + 1] = src[row_offset + dim + j + 1];
        }
    }
    // copy transpose last col and last row if N is odd
    if (dim % 2 == 1) {
        for (int k = 0; k < dim; k++) {
            dest[(dim - 1) * dim + k] = src[dim * k + dim - 1];
            dest[dim * k + dim - 1] = src[(dim - 1) * dim + k];
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

//
//  tester.c
//  Datalab
//
//  Created by Yuanping Song on 4/19/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include <stdio.h>
void show_int(int x);
int negate(int x) {
    return ~x + 1;
}

int anyOddBit(int x) {
    int accumulator = x & 0xAA;
    printf("%i %i \n", x, accumulator);
    show_int(x);
    accumulator += (x >> 8) & 0xAA;
    printf("%i %i \n", x >>8, accumulator);
    show_int(x >> 8);
    accumulator += (x >> 16) & 0xAA;
    printf("%i %i \n", x >> 16, accumulator);
    show_int(x >> 16);
    accumulator += (x >> 24) & 0xAA;
    printf("%i %i \n", x >> 24, accumulator);
    show_int(x >> 24);
    return !!accumulator;
}

int divpwr2(int x, int n) {
    int bias = (1 << n) - 1;
    return (((x >> 31) & bias)  + x) >> n;
}

int addOK(int x, int y) {
    int failScenario1 = ((x >> 31) & 1) & ((y >> 31) & 1) & ((x + y) >> 31 ^ 1);
    int failScenario2 = ((x >> 31) ^ 1) & ((y >> 31) ^ 1) & ((x + y) >> 31 & 1);
    return !(failScenario1 | failScenario2);
}

/*int isGreater(int x, int y) {
    int halfx = x >> 1;
    int halfy = y >> 1;
    return !((((halfx + (~halfy + 1)) >> 31) & 1) | ((((x & 1) + (~(y & 1) + 1)) >> 31) & 1));
}*/

/*int isGreater(int x, int y) {
    int halfx = x >> 1;
    int halfy = y >> 1;
    int operand1 = halfy + (~halfx + 1);
    int operand2 = (y & 1) + (~(x & 1) + 1);
    int condition1 = (operand1 >> 31) & 1; // if this condition is true then x is not greater than y
    int condition2 = !operand1; // if this condition is true, then x is greater than y
    int condition3 =  (operand2 >> 31) & 1; // if the above two conditions are both false, then this condition tells us that x is greater then y when it is ture;
    return true;
}*/

int isGreater(int x, int y) {
    int halfx = x >> 1;
    int halfy = y >> 1;
    int operand1 = halfy + (~halfx + 1);
    int condition1 = operand1 >> 31 & 1; // y - x < 0 ?
    int indeterminate = !operand1;
    int condition2 = (x & 1) & ((x & 1) ^ (y & 1)) ;
    return condition1 | (indeterminate & condition2);
}

int replaceByte(int x, int n, int c) {
    int mask = 0xFF << (n << 3);
    int annilator = ~(mask & x) + 1;
    return x + annilator + (c << (n << 3));
}

int tc2sm(int x) {
    int posMask = ~(x >> 31); // all ones if x is pos
    int negMask = (x >> 31); // all ones if x is neg
    return (posMask & x) + (negMask & (~x + 1) + (1 << 31));
}

typedef unsigned char *byte_pointer;

void show_bytes(byte_pointer start, int len)
{
    int i;
    for (i = 0; i < len; i++)
        printf(" %.2x", start[i]);
    printf("\n");
}

void show_int(int x)
{
    show_bytes((byte_pointer) &x, sizeof(int));
}

int main() {
    //int bounds = 100000;
    //int step = 123;
    //for (int i = -bounds; i < bounds; i += step) {
        //printf("The negation of %i is %i \n", i, negate(i));
        
   // }
    //printf("%i %i",divpwr2(73, 3), divpwr2(-73,3) );
    //printf("%i %i", 0x7, );
    //printf("%i",isGreater(-3, -3));
    show_int(2147483647);
    show_int(tc2sm(2147483647));
    
}

//
//  linear.cpp
//  CS32_HW_3
//
//  Created by Yuanping Song on 2/12/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//
/*#include <cmath>
#include <random>
#include <iostream>
using namespace std;*/

/*bool somePredicate(double x)
{
    return x > 0; // include <cmath> for std::sin, etc.
}*/


// Return false if the somePredicate function returns false for at
// least one of the array elements; return true otherwise.
bool allTrue(const double a[], int n)
{
    if (n <= 0) {
        return true;
    }
    return somePredicate(a[n-1]) && allTrue(a, --n);
}

// Return the number of elements in the array for which the
// somePredicate function returns false.
int countFalse(const double a[], int n)
{
    if (n <= 0) {
        return 0;
    } else  {
        if ( somePredicate(a[n - 1]) ) {
            return countFalse(a, --n);
        } else {
            return 1 + countFalse(a, --n);
        }
    }
}

// Return the subscript of the first element in the array for which
// the somePredicate function returns false.  If there is no such
// element, return -1.
int firstFalse(const double a[], int n)
{
    if (n <= 0) {
        return -1;
    } else {
        if (somePredicate(a[0])) {
            return 1 + firstFalse(a + 1, --n);
        } else {
            return 0;
        }
    }
}

// Return the subscript of the smallest double in the array (i.e.,
// the one whose value is <= the value of all elements).  If more
// than one element has the same smallest value, return the smallest
// subscript of such an element.  If the array has no elements to
// examine, return -1.
int indexOfMin(const double a[], int n)
{
    if (n <= 0) {
        return -1;
    } else {
        if ( n == 1) {
            return 0;
        } else {
            int tIdx = indexOfMin(a, n - 1);
            return a[tIdx] <= a[n - 1] ? tIdx : n - 1;
        }
    }
}

// If all n2 elements of a2 appear in the n1 element array a1, in
// the same order (though not necessarily consecutively), then
// return true; otherwise (i.e., if the array a1 does not include
// a2 as a not-necessarily-contiguous subsequence), return false.
// (Of course, if a2 is empty (i.e., n2 is 0), return true.)
// For example, if a1 is the 7 element array
//    10 50 40 20 50 40 30
// then the function should return true if a2 is
//    50 20 30
// or
//    50 40 40
// and it should return false if a2 is
//    50 30 20
// or
//    10 20 20
bool includes(const double a1[], int n1, const double a2[], int n2)
{
    if ( n2 <= 0 ) {
        return true;
    } else if (n1 <= 0) {
        return false;
    }
    if (a1[n1 - 1] == a2[n2 - 1]) {
        return includes(a1, n1 - 1, a2, n2 - 1);
    } else {
        return includes(a1, n1 - 1, a2, n2);
    }
}
/*
int main() {
    double nums[100];
    for (int i = 0; i < 100; i++) {
        nums[i] = random() % 1000 * pow(-1.0, i);
    }
    for (int i = 0; i < 100; i++) {
        cout << "Index " << i << ": " << nums[i] << endl;
    }
    cout << firstFalse(nums, 100);
    cout << indexOfMin(nums, 100);
    
}*/

//
//  main.cpp
//  Project_6
//
//  Created by Yuanping Song on 11/26/17.
//  Copyright © 2017 Yuanping Song. All rights reserved.
//
#include <iostream>
using namespace std;
// Problem 1A
/*
int main()
{
    int arr[3] = { 5, 10, 15 };
    int* ptr = arr;
    
    *ptr = 30;
    *(ptr + 1) = 20;
    ptr += 2;
    ptr[0] = 10;
    
    for (ptr = arr; ptr < arr + 3; ptr++) {
        cout << *ptr << endl;
    }
}*/

// Problem 1B
/*
void findMax(int arr[], int n, int*& pToMax)
{
    if (n <= 0)
        return;      // no items, no maximum!
    
    pToMax = arr;
    
    for (int i = 1; i < n; i++)
    {
        if (arr[i] > *pToMax)
            pToMax = arr + i;
    }
}


int main()
{
    int nums[4] = { 5, 3, 15, 6 };
    int* ptr;
    
    findMax(nums, 4, ptr);
    cout << "The maximum is at address " << ptr << endl;
    cout << "It's at position " << ptr - nums << endl;
    cout << "Its value is " << *ptr << endl;
}
*/

// Problem 1C
/*
void computeCube(int n, int* ncubed) {
    *ncubed = n * n * n;
}

int main() {
    int cube;
    int * ptr = &cube;
    computeCube(5, ptr);
    cout << "Five cubed is " << *ptr << endl;
}
*/

// Problem 1D
/*
// return true if two C strings are equal
bool strequal(const char str1[], const char str2[])
{
    while (*str1 != 0  &&  *str2 != 0)
    {
        if (*str1 != *str2)  // compare corresponding characters
            return false;
        str1++;            // advance to the next character
        str2++;
    }
    return *str1 == *str2;   // both ended at same time?
}

int main()
{
    char a[15] = "Zhou";
    char b[15] = "Zhu";
    
    if (strequal(a,b))
        cout << "They're the same person!\n";
}
 */
 
// Problem 1
 /*
 #include <iostream>
using namespace std;

int* nochange(int *p) {
    return p;
}

int * getPtrToArray(int &m) {
    int anArray[100];
    for (int j = 0; j < 100; j++) {
        anArray[j] = 100 - j;
     }
    m = 100;
    return nochange(anArray);
}

void f() {
    int junk[100];
    for (int k = 0; k < 100; k++) {
        junk[k] = 123400000 + k;
        junk[50]++;
    }
}

int main(int argc, const char * argv[]) {
    int n;
    int *ptr = getPtrToArray(n);
    f();
    for (int i = 0; i < 3; i++) {
        cout << ptr[i] << ' ';
    }
    for (int i = n - 3; i < n; i++) {
        cout << ptr[i] << ' ';
    }
    cout << endl;
    return 0;
}
*/
// Problem 2
/*
int main() {
    double *cat;
    double mouse[5];
    cat = &mouse[4];
    *cat = 25;
    *(mouse + 3) = 42;
    cat -= 3;
    cat[1] = 54;
    cat[0] = 27;
    bool b = ( *cat == *(cat + 1) );
    bool d = ( cat == mouse );
    return 0;
}
 
 */

// Problem 3A
/*
double mean(const double*, int);
int main() {
    double scores[] {12, 23, 34, 45, 56, 67, 78};
    cout << mean(scores, 7);
    return 0;
}

double mean(const double* scores, int numScores)
{
    
    double tot = 0;
    for (int i = 0; i < numScores; i++) {
        tot += *(scores + i);
    }
    return tot/numScores;
}
 */

// Problem 3B
/*
// This function searches through str for the character chr.
// If the chr is found, it returns a pointer into str where
// the character was first found, otherwise nullptr (not found).
const char* findTheChar(const char *str, char chr)
{
    for (int i = 0; *(str + i) != 0; i++) {
        if ( *(str + i) == chr ) {
            return (str + i);
        }
    }
    return nullptr;
}

int main() {
    const char arr[] = "Hello World";

    cout << *findTheChar(arr, 'H');
    return 0;
}
*/
// Problem 3C
/*
const char* findTheChar(const char *str, char chr)
{
    for (; *str != 0; str++) {
        if ( *str == chr ) {
            return str;
        }
    }
    return nullptr;
}

int main() {
    const char arr[] = "Hello World";
    
    cout << *findTheChar(arr, 'h');
    return 0;
}
 */

// Problem 4
/*
#include <iostream>
using namespace std;

int *  maxwell(int* a, int* b) {
    if (*a > *b ) {
        return a;
    } else {
        return b;
    }
}

void swap1(int *a, int *b) {
    int *temp = a;
    a = b;
    b = temp;
}

void swap2(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int main() {
    int array[6] = { 5, 3, 4, 17, 22, 19 };
    
    int *ptr = maxwell(array, &array[2]);
    *ptr = -1;
    ptr += 2;
    ptr[1] = 9;
    *(array + 1) = 79;
    cout << &array[5] - ptr << endl;
    
    swap1(&array[0], &array[1]);
    swap2(array, &array[2]);
    
    for (int i = 0; i < 6; i++) {
        cout << array[i] << endl;
    }
    
}
*/

//problem 5
/*
#include <iostream>
void removeS (char *param) {
    for (; *param != 0; param++) {
        if (*param == 's' || *param == 'S') {
            for (char *it = param; *it != 0; it++) {
                *it = *(it + 1);
            }
            param--;
        }
    }
}

int main() {
    char arr[] = "She'll be a massssssssssslesss princess.";
    removeS(arr);
    std::cout << arr;
}
*/

//
//  main.cpp
//  Project_4
//
//  Created by Yuanping Song on 11/4/17.
//  Copyright © 2017 Yuanping Song. All rights reserved.
//

#include <iostream>
#include <string>
#include <cassert>
using namespace std;

int appendToAll(string a[], int n, string value);
int lookup(const string a[], int n, string target);
int positionOfMax(const string a[], int n);
int rotateLeft(string a[], int n, int pos);
int countRuns(const string a[], int n);
int flip(string a[], int n);
int differ(const string a1[], int n1, const string a2[], int n2);
int subsequence(const string a1[], int n1, const string a2[], int n2);
int lookupAny(const string a1[], int n1, const string a2[], int n2);
int split(string a[], int n, string splitter);

int main() {
    string people[5] = { "clark", "peter", "diana", "tony", "selina" };
    string multPeople[6] = { "clark", "peter", "diana", "clark", "tony", "selina" };
    
    assert(appendToAll(people, -1, "!!!") == -1 && people[0] == "clark" && people[4] == "selina");
    assert(appendToAll(people, 0, "!!!") == 0 && people[0] == "clark" && people[4] == "selina");
    assert(appendToAll(people, 5, "!!!") == 5 && people[0] == "clark!!!" && people[1] == "peter!!!" &&
           people[2] == "diana!!!" && people[3] == "tony!!!" && people[4] == "selina!!!");
    assert(appendToAll(people, 2, "@@@") == 2 && people[0] == "clark!!!@@@" && people[1] == "peter!!!@@@");
    assert(appendToAll(people, 1, "!!!") == 1 && people[0] == "clark!!!@@@!!!" && people[1] == "peter!!!@@@");
    assert(appendToAll(multPeople, 6, "!") == 6 && multPeople[0] == "clark!" && multPeople[3] == "clark!");
    assert(appendToAll(people, 1, "") == 1 && people[0] == "clark!!!@@@!!!");
    
    
    people[0] = "clark"; people[1] = "peter"; people[2] = "diana"; people[3] = "tony"; people[4] = "selina";
    multPeople[0] = "clark"; multPeople[1] = "peter"; multPeople[2] = "diana"; multPeople[3] = "clark"; multPeople[4] = "tony"; multPeople[5] = "selina";
    
    assert(lookup(people, -1, "clark") == -1);
    assert(lookup(people, 0, "clark") == -1);
    assert(lookup(people, 5, "diana") == 2);
    assert(lookup(people, 2, "selina") == -1);
    assert(lookup(people, 1, "clark") == 0);
    assert(lookup(people, 1, "peter") == -1);
    assert(lookup(multPeople, 6, "clark") == 0);
    
    
    string hero[6] = { "clark", "peter", "reed", "tony", "diana", "bruce" };
    string multHero[7] = { "tony", "clark", "peter", "reed", "tony", "diana", "bruce" };
    
    assert(positionOfMax(hero, -1) == -1);
    assert(positionOfMax(hero, 0) == -1);
    assert(positionOfMax(hero, 6) == 3);
    assert(positionOfMax(hero, 3) == 2);
    assert(positionOfMax(hero, 1) == 0);
    assert(positionOfMax(multHero, 7) == 0);
    
    
    
    string super[5] = { "logan", "clark", "peter", "sue", "reed" };
    people[0] = "clark"; people[1] = "peter"; people[2] = "diana"; people[3] = "tony"; people[4] = "selina";
    multPeople[0] = "clark"; multPeople[1] = "peter"; multPeople[2] = "diana"; multPeople[3] = "clark"; multPeople[4] = "tony"; multPeople[5] = "selina";
    
    assert(rotateLeft(people, -1, 1) == -1 && people[0] == "clark" && people[4] == "selina");
    assert(rotateLeft(people, 0, 1) == -1 && people[0] == "clark" && people[4] == "selina");
    assert(rotateLeft(super, 5, 1) == 1 && super[0] == "logan" && super[1] == "peter" && super[2] == "sue" && super[3] == "reed" && super[4] == "clark");
    assert(rotateLeft(super, 3, 1) == 1 && super[0] == "logan" && super[1] == "sue" && super[2] == "peter" && super[3] == "reed" && super[4] == "clark");
    assert(rotateLeft(super, 1, 0) == 0 && super[0] == "logan" && super[4] == "clark");
    assert(rotateLeft(super, 1, 1) == -1 && super[0] == "logan" && super[4] == "clark");
    assert(rotateLeft(super, 5, 4) == 4 && super[0] == "logan" && super[4] == "clark");
    assert(rotateLeft(multPeople, 6, 3) == 3 && multPeople[0] == "clark" && multPeople[1] == "peter" && multPeople[2] == "diana" && multPeople[3] == "tony" && multPeople[4] == "selina" && multPeople[5] == "clark");
    
    
    people[0] = "clark"; people[1] = "peter"; people[2] = "diana"; people[3] = "tony"; people[4] = "selina";
    multPeople[0] = "clark"; multPeople[1] = "peter"; multPeople[2] = "diana"; multPeople[3] = "clark"; multPeople[4] = "tony"; multPeople[5] = "selina";
    string d[9] = { "tony", "bruce", "steve", "steve", "diana", "diana", "diana", "steve", "steve" };
    assert(countRuns(people, -1) == -1);
    assert(countRuns(people, 0) == 0);
    assert(countRuns(d, 9) == 5);
    assert(countRuns(d, 4) == 3);
    assert(countRuns(d, 1) == 1);
    assert(countRuns(multPeople, 6) == 6);
    
    
    string folks[6] = { "bruce", "steve", "", "tony", "sue", "clark" };
    assert(flip(people, -1) == -1 && folks[0] == "bruce" && folks[5] == "clark");
    assert(flip(people, 0) == 0 && folks[0] == "bruce" && folks[5] == "clark");
    assert(flip(people, -1) == -1 && folks[0] == "bruce" && folks[5] == "clark");
    assert(flip(folks, 4) == 4 && folks[0] == "tony" && folks[1] == "" && folks[2] == "steve" && folks[3] == "bruce" && folks[4] == "sue" && folks[5] == "clark");
    assert(flip(folks, 6) == 6 && folks[5] == "tony" && folks[4] == "" && folks[3] == "steve" && folks[2] == "bruce" && folks[1] == "sue" && folks[0] == "clark");
    assert(flip(folks, 1) == 1 && folks[5] == "tony" && folks[0] == "clark");
    
    
    
    folks[0] = "bruce"; folks[1] = "steve"; folks[2] = ""; folks[3] = "tony"; folks[4] = "sue"; folks[5] = "clark";
    string group[5] = { "bruce", "steve", "clark", "", "tony" };
    assert(differ(folks, -1, group, 1) == -1);
    assert(differ(folks, 1, group, -1) == -1);
    assert(differ(folks, 0, group, 1) == 0);
    assert(differ(folks, 1, group, 0) == 0);
    assert(differ(folks, 2, group, 1) == 1);
    assert(differ(folks, 6, group, 5) == 2);
    assert(differ(folks, 6, folks, 4) == 4);
    assert(differ(group, 3, group, 5) == 3);
    
    
    string names[10] = { "logan", "reed", "sue", "selina", "bruce", "peter", "logan" };
    string names1[10] = { "reed", "sue", "selina" };
    string names2[10] = { "logan", "selina" };
    assert(subsequence(names, -1, names1, 1) == -1);
    assert(subsequence(names, 1, names1, -1) == -1);
    assert(subsequence(names, 0, names1, 1) == -1);
    assert(subsequence(names, 1, names1, 0) == 0);
    assert(subsequence(names, 0, names1, 0) == 0);
    assert(subsequence(names, 6, names1, 3) == 1);
    assert(subsequence(names, 5, names2, 2) == -1);
    assert(subsequence(names, 1, names2, 1) == 0);
    assert(subsequence(names, 7, names2, 1) == 0);
    
    names[6] = "";
    string set1[10] = { "clark", "bruce", "selina", "reed" };
    string set2[10] = { "tony", "diana" };
    assert(lookupAny(names, -1, names1, 1) == -1);
    assert(lookupAny(names, 1, names1, -1) == -1);
    assert(lookupAny(names, 0, names1, 1) == -1);
    assert(lookupAny(names, 1, names1, 0) == -1);
    assert(lookupAny(names, 0, names1, 0) == -1);
    assert(lookupAny(names, 6, set1, 4) == 1);
    assert(lookupAny(names, 6, set2, 2) == -1);
    assert(lookupAny(names, 1, set1, 4) == -1);
    assert(lookupAny(multPeople, 6, set1, 4) == 0);
    
    // hero still defined as { "clark", "peter", "reed", "tony", "diana", "bruce" }
    // multPeople still defined as { "clark", "peter", "diana", "clark", "tony", "selina" };
    string hero2[4] = { "reed", "sue", "peter", "steve" };
    string hero3[2] = {"abc", "def"};
    
    assert(split(hero, -1, "logan") == -1 && hero[0] == "clark" && hero[5] == "bruce");
    assert(split(hero, 0, "logan") == 0 && hero[0] == "clark" && hero[5] == "bruce");
    assert(split(hero, 0, "clark") == 0 && hero[0] == "clark" && hero[5] == "bruce");
    assert(split(hero2, 1, "reed") == 0 && hero2[0] == "reed" && hero2[3] == "steve");
    assert(split(hero, 6, "logan") == 3);
    assert(split(hero2, 4, "steve") == 2 && hero2[2] == "steve");
    assert(split(multPeople, 6, "clark") == 0 && multPeople[0] == "clark" && multPeople[1] == "clark");
    assert(split(hero3, 2, "xyz") == 2 && hero3[0] == "abc" && hero3[1] == "def");
    
    
    string h[7] = { "selina", "reed", "diana", "tony", "", "logan", "peter" };
    assert(lookup(h, 7, "logan") == 5);
    assert(lookup(h, 7, "diana") == 2);
    assert(lookup(h, 2, "diana") == -1);
    assert(positionOfMax(h, 7) == 3);
    
    string g[4] = { "selina", "reed", "peter", "sue" };
    assert(differ(h, 4, g, 4) == 2);
    assert(appendToAll(g, 4, "?") == 4 && g[0] == "selina?" && g[3] == "sue?");
    assert(rotateLeft(g, 4, 1) == 1 && g[1] == "peter?" && g[3] == "reed?");
    
    string e[4] = { "diana", "tony", "", "logan" };
    assert(subsequence(h, 7, e, 4) == 2);
    
    string c[5] = { "reed", "reed", "reed", "tony", "tony" };
    assert(countRuns(c, 5) == 2);
    
    string f[3] = { "peter", "diana", "steve" };
    assert(lookupAny(h, 7, f, 3) == 2);
    assert(flip(f, 3) == 3 && f[0] == "steve" && f[2] == "peter");
    
    assert(split(h, 7, "peter") == 3);
    
    cerr << "All tests succeeded" << endl;
    return 0;
}


/*
 int appendToAll(string a[], int n, string value);
 Append value to the end of each of the n elements of the array and return n. [Of course, in this and other functions, if n is negative, the paragraph above that starts "Notwithstanding" trumps this by requiring that the function return −1. Also, in the description of this function and the others, when we say "the array", we mean the n elements that the function is aware of.] For example:
 string people[5] = { "clark", "peter", "diana", "tony", "selina" };
 int j = appendToAll(people, 5, "!!!");  // returns 5
 // now people[0] is "clark!!!", people[1] is "peter!!!", ...,
 // and people[4] is "selina!!!"
 */

int appendToAll(string a[], int n, string value) {
    if (n < 0) {
        return -1;
    } else {
        for (int i = 0; i < n ; i++) {
            a[i] += value; // string concatenation
        }
        return n;
    }
}

/*
 Return the position of a string in the array that is equal to target; if there is more than one such string, return the smallest position number of such a matching string. Return −1 if there is no such string. As noted above, case matters: Do not consider "SUe" to be equal to "sUE".
 */

int lookup(const string a[], int n, string target){
    if (n < 0) {
        return -1;
    } else {
        for (int i = 0; i < n; i++) {
            if (a[i] == target) {
                return i;
            }
        }
        return -1;
    }
}
/*
 Return the position of a string in the array such that that string is >= every string in the array. If there is more than one such string, return the smallest position in the array of such a string. Return −1 if the array has no elements. For example:
 string hero[6] = { "clark", "peter", "reed", "tony", "diana", "bruce" };
 int k = positionOfMax(hero, 6);   // returns 3, since  tony  is latest
 // in alphabetic order
 */
int positionOfMax(const string a[], int n){
    if (n < 0) {
        return -1;
    } else {
        string max;
        int index = -1; // index keeps track the position of max in the array. it is initialized to -1 so that if there is no element in the array, this value is returned.
        for (int i = 0; i < n; i++) {
            if(max.empty()){ // during the first iteration, max will be initialized with the fist element of the array, if it has any such element.
                max = a[i];
                index = i;
            } else if (a[i] > max) {
                max = a[i]; // update max
                index = i; // update index
            }
        }
        return index;
    }
}

/*
 int rotateLeft(string a[], int n, int pos);
 Eliminate the item at position pos by copying all elements after it one place to the left. Put the item that was thus eliminated into the last position of the array. Return the original position of the item that was moved to the end. Here's an example:
 string super[5] = { "logan", "clark", "peter", "sue", "reed" };
 int m = rotateLeft(super, 5, 1);  // returns 1
 // super now contains:  "logan", "peter", "sue", "reed", "clark"
 */

int rotateLeft(string a[], int n, int pos){
    if (n < 0 || pos >= n) {
        return -1;
    } else {
        for (int i = pos; i + 1 < n; i++) { // this is one iteration of bubble sort without the sorting
            string temp = a[i];
            a[i] = a[i + 1];
            a[i + 1] = temp;
        }
        return pos;
    }
}


/*
 int countRuns(const string a[], int n);
 Return the number of sequences of one or more consecutive identical items in a.
 string d[9] = {
 "tony", "bruce", "steve", "steve", "diana", "diana", "diana", "steve", "steve"
 };
 int p = countRuns(d, 9);  //  returns 5
 //  The five sequences of consecutive identical items are
 //    "tony"
 //    "bruce"
 //    "steve", "steve"
 //    "diana", "diana", "diana"
 //    "steve", "steve"
 */

int countRuns(const string a[], int n){
    if(n < 0) {
        return -1;
    } else if (n == 0) {
        return 0;
    } else {
        int counter = 1;
        for (int i = 0; i + 1 < n; i++) {
            if (a[i] != a[i+1]) { // count the number of changes
                counter++;
            }
        }
        return counter;
    }
}

/*
 int flip(string a[], int n);
 Reverse the order of the elements of the array and return n. For example,
 string folks[6] = { "bruce", "steve", "", "tony", "sue", "clark" };
 int q = flip(folks, 4);  // returns 4
 // folks now contains:  "tony"  ""  "steve"  "bruce"  "sue"  "clark"
 */

int flip(string a[], int n){
    if (n < 0) {
        return -1;
    } else if(n == 0 || n == 1) {
        return n; // empty array and array with only one element do not need to be flipped.
    }
    else {
        for (int i = 0; i < n / 2; i++) { // flip n/2 times. if n is even, every element is swaped. if n is odd, the middle element is left alone. This desired behavior is brought about by integer division.
            string temp;
            temp = a[i];
            a[i] = a[n - i - 1]; // we can do this becauae at this point n is at least 2. n - 1 yields the index of the last element in the array and i and -i are the offsets.
            a[n - i - 1] = temp;
        }
        return n;
    }
}

/*
 int differ(const string a1[], int n1, const string a2[], int n2);
 Return the position of the first corresponding elements of a1 and a2 that are not equal. n1 is the number of interesting elements in a1, and n2 is the number of interesting elements in a2. If the arrays are equal up to the point where one or both runs out, return whichever value of n1 and n2 is less than or equal to the other. For example,
 string folks[6] = { "bruce", "steve", "", "tony", "sue", "clark" };
 string group[5] = { "bruce", "steve", "clark", "", "tony" };
 int r = differ(folks, 6, group, 5);  //  returns 2
 int s = differ(folks, 2, group, 1);  //  returns 1
 */

int differ(const string a1[], int n1, const string a2[], int n2){
    if (n1 < 0 || n2 < 0) {
        return -1;
    }
    for (int i = 0; i < min(n1, n2); i++) { // compare two arrays until one runs out
        if (a1[i] != a2[i]) {
            return i;
        }
    }
    return min(n1, n2);
}

/*
 int subsequence(const string a1[], int n1, const string a2[], int n2);
 If all n2 elements of a2 appear in a1, consecutively and in the same order, then return the position in a1 where that subsequence begins. If the subsequence appears more than once in a1, return the smallest such beginning position in the array. Return −1 if a1 does not contain a2 as a contiguous subsequence. (Consider a sequence of 0 elements to be a subsequence of any sequence, even one with no elements, starting at position 0.) For example,
 string names[10] = { "logan", "reed", "sue", "selina", "bruce", "peter" };
 string names1[10] = { "reed", "sue", "selina" };
 int t = subsequence(names, 6, names1, 3);  // returns 1
 string names2[10] = { "logan", "selina" };
 int u = subsequence(names, 5, names2, 2);  // returns -1
 */

int subsequence(const string a1[], int n1, const string a2[], int n2) {
    if (n1 < 0 || n2 < 0 || (n2 > n1)) {
        return -1;
    }
    if(n2 == 0) {
        return 0;
    }
    for (int i = 0; (i < n1) && (i + n2 <= n1); i++) { // the second condition checks to see if it is possible to fit a2 in the unexamined portion of a1.
        bool isEqual = true;
        for (int j = 0; j < n2; j++) {
            if (a1[i+j] != a2[j]) {
                isEqual = false;
                break;
            }
        }
        if (isEqual) {
            return i;
        }
    }
    return -1;
}

/*
 int lookupAny(const string a1[], int n1, const string a2[], int n2);
 Return the smallest position in a1 of an element that is equal to any of the elements in a2. Return −1 if no element of a1 is equal to any element of a2. For example,
 string names[10] = { "logan", "reed", "sue", "selina", "bruce", "peter" };
 string set1[10] = { "clark", "bruce", "selina", "reed" };
 int v = lookupAny(names, 6, set1, 4);  // returns 1 (a1 has "reed" there)
 string set2[10] = { "tony", "diana" };
 int w = lookupAny(names, 6, set2, 2);  // returns -1 (a1 has none)
 */
int lookupAny(const string a1[], int n1, const string a2[], int n2){
    if (n1 < 0 || n2 < 0) {
        return -1;
    }
    for (int i = 0; i < n1; i++) {
        for(int j = 0; j < n2; j++){
            if (a1[i] == a2[j]) {
                return i;
            }
        }
    }
    return -1;
}

/*
 int split(string a[], int n, string splitter);
 Rearrange the elements of the array so that all the elements whose value is < splitter come before all the other elements, and all the elements whose value is > splitter come after all the other elements. Return the position of the first element that, after the rearrangement, is not < splitter, or n if there are no such elements. For example,
 string hero[6] = { "clark", "peter", "reed", "tony", "diana", "bruce" };
 int x = split(hero, 6, "logan");  //  returns 3
 // hero must now be
 //      "clark"  "diana"  "bruce"  "peter"  "tony"  "reed"
 // or   "diana"  "bruce"  "clark"  "reed"  "peter"  "tony"
 // or one of several other orderings.
 // All elements < "logan" (i.e., "diana", "bruce", and "clark")
 //   come before all others
 // All elements > "logan" (i.e., "tony", "peter", and "reed")
 //   come after all others
 string hero2[4] = { "reed", "sue", "peter", "steve" };
 int y = split(hero2, 4, "steve");  //  returns 2
 // hero2 must now be either
 //      "reed"  "peter"  "steve"  "sue"
 // or   "peter"  "reed"  "steve"  "sue"
 // All elements < "steve" (i.e., "peter" and "reed") come
 // before all others.
 // All elements > "steve" (i.e., "sue") come after all others
 */

int split(string a[], int n, string splitter){
    if(n < 0){
        return -1;
    }
    for (int i = 0; i < n; i++) { // bubble sort, put the array into order first.
        for (int j = 0; j + 1 < n - i; j++) {
            if(a[j] > a[j + 1]) {
                string temp = a[j + 1];
                a[j + 1] = a[j];
                a[j] = temp;
            }
        }
    }
    
    for (int i = 0; i < n ; i++) { // find the index of the first element that is greater than or equal to the splitter.
        if ( !(a[i] < splitter) ) {
            return i;
        }
    }
    return n; // if there is no such element, return n.
}


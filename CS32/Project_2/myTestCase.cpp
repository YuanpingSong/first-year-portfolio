//
//  main.cpp
//  Project_2
//
//  Created by Yuanping Song on 1/28/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include <iostream>
#include <string>
#include <cassert>
#include <cstdlib>
#include <time.h>
using namespace std;
#include "Map.h"

int main(int argc, const char * argv[]) {
    
    
    string keyArr[] = {"Alpha", "Bravo", "Charlie", "Delta", "Echo", "Foxtrot", "Gulf", "Hotel", "India", "Juliet", "Kilo", "Lima", "Mike", "November", "Oscar", "Papa", "Quebec", "Romeo", "Sierra", "Tango", "Uniform", "Victor", "Whiskey", "Xray", "Yankee", "Zulu" };
    
    // default contructor
    Map myMap;
    assert(myMap.size() == 0);
    assert(myMap.empty());
    assert(!myMap.erase("Ricky"));
    
    // Copy Constructor
    Map mySecondMap(myMap);
    assert(mySecondMap.size() == 0);
    assert(mySecondMap.empty());
    assert(!mySecondMap.erase("Ricky"));
    // insert or update will always succeed;
    for (int i = 0; i < 10000; i++) {
        assert(mySecondMap.insertOrUpdate(keyArr[i % 26], i));
    }
    
    // empty map can be assigned
    mySecondMap = myMap;
    assert(mySecondMap.size() == 0);
    assert(mySecondMap.empty());
    assert(!mySecondMap.erase("Ricky"));
    // insert 26 elements into map
    for (int i = 0; i < 26; i++) {
        assert(myMap.insert(keyArr[i], i));
    }
    // Map should not allow duplicate keys
    for (int i = 0; i < 26; i++) {
        assert(!myMap.insert(keyArr[i], i));
    }
    
    // the following code tests combine
    int cutOff = 12;
    
    Map subMap1;
    for (int i = 0; i < cutOff; i++) {
        subMap1.insert(keyArr[i], i);
    }
    
    Map subMap2;
    for (int i = cutOff; i < 26; i++) {
        subMap2.insert(keyArr[i], i);
    }
    Map result;
    // combining two non-overlapping maps should always succeed.
    assert(combine(subMap1, subMap2, result));
    
    // checks if result is indeed the union of two submaps
    for (int i = 0; i < 26; i++) {
        assert(result.contains(keyArr[i]));
    }
    subMap1.insert(keyArr[25], 12);
    
    // submaps contain conflicting key-value pairs, combine should return false
    assert(!combine(subMap1, subMap2, result));
    
    // the conflicted key should be removed from result
    assert(!result.contains(keyArr[25]));
    
    // the other keys should make it into result
    for (int i = 0; i < 25; i++) {
        assert(result.contains(keyArr[i]));
    }
    
    // resolve the cause of the conflict
    assert(subMap1.update(keyArr[25], 25));
    assert(combine(subMap1, subMap2, result) && result.size() == 26 && result.contains(keyArr[25]));
    
    // the following code tests subtract
    subtract(result, subMap2, result);
    assert(result.size() == 12);
    for (int i = 0; i < 12; i++) {
        assert(result.contains(keyArr[i]));
    }
    
    // the following code tests the validity of Map's implementation
    
    //#define GET_ITERATION_TEST
#ifdef GET_ITERATION_TEST
    Map m;
    m.insert("A", 10);
    m.insert("B", 44);
    m.insert("C", 10);
    string all;
    double total = 0;
    for (int n = 0; n < m.size(); n++)
    {
        string k;
        double v;
        m.get(n, k, v);
        all += k;
        total += v;
    }
    cout << all << total;
#endif
    
    //#define GET_CONSISTENCY_TEST
#ifdef GET_CONSISTENCY_TEST
    Map gpas;
    gpas.insert("Fred", 2.956);
    gpas.insert("Ethel", 3.538);
    double v;
    string k1;
    assert(gpas.get(1,k1,v)  &&  (k1 == "Fred"  ||  k1 == "Ethel"));
    string k2;
    assert(gpas.get(1,k2,v)  &&  k2 == k1);
#endif
    
    //#define EMPTY_STRING_TEST
#ifdef EMPTY_STRING_TEST
    Map gpas;
    gpas.insert("Fred", 2.956);
    assert(!gpas.contains(""));
    gpas.insert("Ethel", 3.538);
    gpas.insert("", 4.000);
    gpas.insert("Lucy", 2.956);
    assert(gpas.contains(""));
    gpas.erase("Fred");
    assert(gpas.size() == 3  &&  gpas.contains("Lucy")  &&  gpas.contains("Ethel")  &&
           gpas.contains(""));
#endif
    
    //#define SWAP_TEST
#ifdef SWAP_TEST
    Map m1;
    m1.insert("Fred", 2.956);
    Map m2;
    m2.insert("Ethel", 3.538);
    m2.insert("Lucy", 2.956);
    m1.swap(m2);
    assert(m1.size() == 2  &&  m1.contains("Ethel")  &&  m1.contains("Lucy")  &&
           m2.size() == 1  &&  m2.contains("Fred"));
#endif
    
    //#define STRING_TO_DOUBLE_TEST
#ifdef STRING_TO_DOUBLE_TEST
    Map m;  // maps strings to doubles
    assert(m.empty());
    ValueType v = -1234.5;
    assert( !m.get("abc", v)  &&  v == -1234.5); // v unchanged by get failure
    m.insert("xyz", 9876.5);
    assert(m.size() == 1);
    KeyType k = "hello";
    assert(m.get(0, k, v)  &&  k == "xyz"  &&  v == 9876.5);
    cout << "Passed all tests" << endl;
    // assignment operator test;
    Map newM = m;
    newM.dump();
    // copy constructor test
    Map nnewM(m);
    nnewM.dump();
    
#endif
    
    
    //#define TEST_N
#ifdef TEST_N
    Map m;
    assert(m.insert("Fred", 2.956));
    assert(m.insert("Ethel", 3.538));
    assert(m.size() == 2);
    ValueType v = 42;
    assert(!m.get("Lucy", v)  &&  v == 42);
    assert(m.get("Fred", v)  &&  v == 2.956);
    v = 42;
    KeyType x = "Lucy";
    assert(m.get(0, x, v)  &&
           ((x == "Fred"  &&  v == 2.956)  ||  (x == "Ethel"  &&  v == 3.538)));
    KeyType x2 = "Ricky";
    assert(m.get(1, x2, v)  &&
           ((x2 == "Fred"  &&  v == 2.956)  ||  (x2 == "Ethel"  &&  v == 3.538))  &&
           x != x2);
#endif
    
    
    
}

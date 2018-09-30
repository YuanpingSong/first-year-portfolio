//
//  tester.cpp
//  Project_2
//
//  Created by Yuanping Song on 2/2/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include <iostream>
using std::cout;
using std::cin;
using std::endl;

int main() {
    cout << "Please enter a number: ";
    int candidate, num, temp = 0;
    cin >> candidate;
    num = candidate;
    while (num != 0) {
        int x = num % 10;
        num /= 10;
        temp = temp * 10 + x;
    }
    
    if(temp == candidate) {
        cout << endl << temp << " is a palindromic number!";
    }
}

//
//  main.cpp
//  Project_2
//
//  Created by Yuanping Song on 10/11/17.
//  Copyright © 2017 Yuanping Song. All rights reserved.
//

#include <iostream>
#include <string>

using namespace std;

// Function Declaration
void printDividingLine();
int inputAge(int& age);
int inputStudentStatus(bool& isStudent);
int inputDestination(string& destination);
int inputZoneCrossings(int& zoneCrossing);
double calculateBaseFee(int age, int zoneCrossing, bool isStudent);

// Global Variable Declaration & Initialization
const int ERR_CODE = 1;

int main() {
    // Local variable declaration
    string destination;
    int zoneCrossing;
    int age;
    bool isStudent;
    
    // Program short circuits if encounters error, do nothing otherwise
    if(inputAge(age) == ERR_CODE) {
        return 1;
    }
    if (inputStudentStatus(isStudent) == ERR_CODE) {
        return 1;
    }
    
    if (inputDestination(destination) == ERR_CODE) {
        return 1;
    }
    
    if (inputZoneCrossings(zoneCrossing) == ERR_CODE) {
        return 1;
    }
    
    // output fare in correct monetary format
    cout.setf(ios::fixed);
    cout.precision(2);
    
    // Print Ticket
    printDividingLine();
    cout << "The fare to " << destination << " is $" << calculateBaseFee(age, zoneCrossing,isStudent) << endl;
    
    return 0;
}

// Print age prompt, read age, return 1 if age is negative, return 0 otherwise
int inputAge(int& age) { // pass by reference argument, this allows the function to modify the argument and pass it back to the caller, main. the following three functions do likewise. 
    // Read from inputstream
    cout << "Age of rider: ";
    cin >> age;
    cin.ignore(100000, '\n');
    if(age < 0) {
        // Enter here if input is not reasonable
        printDividingLine();
        cout << "The age must not be negative\n";
        return ERR_CODE; // 1 indicates failure
    }
    // 0 indicates success
    return 0;
}

// Print student status prompt, read status, return 1 if failure, 0 if success.
int inputStudentStatus(bool& isStudent){
    string temp;
    cout << "Student? (y/n): ";
    cin >> temp;
    cin.ignore(100000, '\n');
    if(temp == "n"){
        
        isStudent = false;
    } else if (temp == "y") {
        isStudent = true;
    } else {
        printDividingLine();
        cout << "You must enter y or n\n";
        return ERR_CODE;
    }
    return 0;
}

// Print destination prompt, read destination, return 1 if failure, 0 if success.
int inputDestination(string& destination){
    cout << "Destination: ";
    getline(cin, destination);
    if (destination == "") {
        printDividingLine();
        cout << "You must enter a destination\n";
        return ERR_CODE;
    }
    return 0;
}

// Print zone crossing prompt, zone crossings, return 1 if failure, 0 if success.
int inputZoneCrossings(int& zoneCrossings){
    cout << "Number of zone boundaries crossed: ";
    cin >> zoneCrossings;
    if (zoneCrossings < 0) {
        printDividingLine();
        cout << "The number of zone boundaries crossed must not be negative\n";
        return ERR_CODE;
    }
    return 0;
}

void printDividingLine() {
    cout << "---\n";
}

// Precondition: age, zoneCrossing, isStudent have been vetted by their input functions
double calculateBaseFee(int age, int zoneCrossing, bool isStudent) {
    double base;
    double rate;
    
    if(age >= 65 && zoneCrossing == 0) {
        return 0.45;
    } else if (zoneCrossing < 2 && (age < 18 || isStudent)) {
        return 0.65;
    } else if (age >= 65) {
        base = 0.55;
        rate = 0.25;
    } else {
        base = 1.35;
        rate = 0.55;
    }
    return base + rate * zoneCrossing;
}


//
//  main.cpp
//  Project_5
//
//  Created by Yuanping Song on 11/16/17.
//  Copyright © 2017 Yuanping Song. All rights reserved.
//

#include <iostream>
#include <cstring>
#include <cctype>
#include <limits>
#include "utilities.h"
using namespace std;



const char FILE_PATH[] = "/Users/yuanpingsong/Desktop/word_bank_7265.txt";
const int ARRAY_SIZE = 10000, MAX_C_STRING_LENGTH = 7, ERROR_CODE = 1;

int runOneRound(const char words[][MAX_C_STRING_LENGTH], int nWords, int wordnum);

/*int main(int argc, const char * argv[]) {

    char words[ARRAY_SIZE][MAX_C_STRING_LENGTH];
    int nWords;
    // load words into array and store the number of words
    nWords = getWords(words, ARRAY_SIZE, FILE_PATH);
    // if no word is loaded, terminate
    if (nWords < 1) {
        cout << "No words were loaded, so I can't play the game.";
        return ERROR_CODE; // terminate immediately
    } else if (nWords <= ARRAY_SIZE) { // if the words can fit in the array
        int roundNum = 0, roundCount = 1;
        int minScore = numeric_limits<int>::max();
        int maxScore = numeric_limits<int>::min();
        double runningTotal = 0.0;
        cout << "How many rounds do you want to play? "; // prompt reader for the number of rounds
        cin >> roundNum;
        cin.ignore(101, '\n'); // discard the newline character or anything else other than the number
        if (roundNum <= 0) { // check if number is positive
            cout << "The number of rounds must be positive.";
            return ERROR_CODE; // terminate immediately
        }
        while (roundCount <= roundNum) { // repeatedly
            cout << endl;
            cout << "Round " << roundCount << endl; // output round number
            int rand = randInt(0, nWords - 1); // randomly select secret word
            int secretWordLength = static_cast<int>( strlen( words[rand] ) );
            cout << "The secret word is " << secretWordLength << " letters long." << endl; // output length of secret word
            int numTries = runOneRound(words, nWords, rand); // play one round and record score
            if (minScore > numTries) {
                minScore = numTries;
            }
            if (maxScore < numTries) {
                maxScore = numTries;
            }
            runningTotal += numTries;
            
            if (numTries == 1) { // inform the reader how many tires he/she took
                cout << "You got it in 1 try." << endl;
            } else {
                cout << "You got it in " << numTries << " tries." << endl;
            }
            cout.setf(ios::fixed);
            cout.precision(2);
            cout << "Average: " << runningTotal / roundCount << ", minimum: " << minScore << ", maximum: " << maxScore << endl; // output stats
            roundCount++;
        }
    } else {
        cerr << "getWords returned more words than the array can hold. Impossible! if the spec did not deceive me."; // impossible
    }
}*/
 
int main() {
    char words[ARRAY_SIZE][MAX_C_STRING_LENGTH];
    int nwords = getWords(words, ARRAY_SIZE, FILE_PATH);
    cout << runOneRound(words, nwords, 18);
}
int runOneRound(const char words[][MAX_C_STRING_LENGTH], int nWords, int wordnum) {
    
    
    if (nWords < 1 || wordnum < 0 || wordnum >= nWords) { // check for bad arguments
        return -1;
    }
    
    auto secretWord = words[wordnum]; // find secret word
    cout << "secretWord is " << secretWord << endl;
    const int MAX_INPUT_LENGTH = 101;
    char input[MAX_INPUT_LENGTH] = {}; // value initialized to null characters
    int counter = 0; // count the number of attempts made i.e. score.
   
    while ( true ) { // repeatedly
        cout << "Probe word: "; // prompt reader for probe word
        cin.getline(input,MAX_INPUT_LENGTH - 1);
        bool isLegalInput = !(strlen(input) < 4 || strlen(input) > 6); // is the length of the input bewteen 4 to 6 inclusive? check if probe meet specs
        for (int i = 0; i < strlen(input); i++) {
            if ( !( isalpha( input[i] ) && islower( input[i] ) ) ) { // is it composed entirely of lower case letters?
                isLegalInput = false;
                break;
            }
        }
        
        if ( !isLegalInput ) { // if either of the requirement is false, inform the player
            cout << "Your probe word must be a word of 4 to 6 lower case letters.\n";
            continue;
        } else {
            bool isLegalWord = false;
            for (int i = 0; i < nWords; i++) {
                if ( strcmp(words[i], input) == 0 ) {
                    isLegalWord = true;
                    break;
                }
            }
            if (!isLegalWord) {
                cout << "I don't know that word.\n";
                continue;
            }
            
            if(strcmp(secretWord, input) == 0) { // if input equals secret word, break
                counter++;
                break;
            }
            
            int starNum = 0, planetNum = 0; // determine the number of stars and planets
            for (int i = 0; i < min(strlen(input), strlen(secretWord)); i++) {
                if (input[i] == secretWord[i]) {
                    starNum++;
                }
            }
            
            for (char c = 'a'; c <= 'z'; c++) { // char is an integral type, so you are allowed to do this
                int countInInput = 0, countInSecretWord = 0;
                for (int i = 0; i < strlen(input); i++) {
                    if(input[i] == c) {
                        countInInput++;
                    }
                }
                for (int i = 0; i < strlen(secretWord); i++) {
                    if(secretWord[i] == c) {
                        countInSecretWord++;
                    }
                }
                for (int i = 0; i < min(strlen(input), strlen(secretWord)); i++) {
                    if(secretWord[i] == c && input[i] == c) {
                        countInSecretWord--;
                        countInInput--;
                    }
                }
                planetNum += min(countInSecretWord, countInInput);
            }
            cout << "Stars: " << starNum << ", Planets: " << planetNum << endl; // output the number of stars and planets
            counter++;
        }
        
        
    } // repeat if user input is not equal to secret word
    
    return counter; // return the number of rounds
}



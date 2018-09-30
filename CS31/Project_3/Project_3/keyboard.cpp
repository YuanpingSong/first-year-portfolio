//
//  main.cpp
//  Project_3
//
//  Created by Yuanping Song on 10/20/17.
//  Copyright © 2017 Yuanping Song. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <vector>
using namespace std;

bool parseSongString(const string& candidate, vector<vector<vector<char>>>& beats);
bool parseBeatString(const string& candidate, vector<vector<char>>& beat);
bool parseNoteString(const string& candidate, vector<char>& note);
bool isAllowedLetter(const char& candidate);
bool isAllowedDigit(const char& candidate);
bool isAccidental(const char& candidate);

int encodeSong(string song, string& instructions, int& badBeat);
bool hasCorrectSyntax(string song);

char encodeNote(int octave, char noteLetter, char accidentalSign);

int main() {
    string song = "G3B3DD5//G/A/A3B/C5/B3D5//G//G//CE5//C5/D5/E5/F#9/B3G5//G//G/";
    string instructions = "Follow instructions: Day Day UP!";
    
    int badBeat = -99901343;
    
    int returnValue = 10000;
    
    returnValue = encodeSong(song, instructions, badBeat);
    
    if (returnValue == 0) {
        cout << song << " is encoded to \n";
        cout << instructions << endl;
    } else if (returnValue == 1) {
        cout << song << " is not a valid song string" << endl;
        cout << "instructions is not modified:  " << instructions << endl;
        cout << "nor is badBeat " << badBeat << endl;
    } else if (returnValue == 2) {
        cout << song << "\nhas the correct syntax but has an unplayable note at position " << badBeat << endl;
        cout << "instructions is not modified:  " << instructions << endl;
    } else {
        cout << "this should not happen. There is an error in your program!!!";
    }
    
    if (instructions.empty() && song.empty()) {
        cout << "We passed the empty string test!" << endl;
    }
    return 0;
}

// give a string, a string reference, and an int reference, returns 1 if the string is not a valid song string.
// returns 2 if the string is a valid song string but has one or more unplayable notes and changes the value of
// badBeat to the index (starting from 1) of the fisrt occurance of unplayable beat.
// returns 0 if the string is a valid song string and constains all playable notes and changes the value of the
// string reference to the encoding of the original string.

int encodeSong(string song, string& instructions, int& badBeat){
    vector<vector<vector<char>>> beats;
    if (!parseSongString(song, beats)) {
        return 1;  // not a songString, both instructions and badBeat unchanged.
    } else {
        string encodedString;
        for (int i = 0; i < beats.size(); i++) {
            if (beats[i].size() == 0){
                encodedString.push_back(' '); // if a beat has no note, then it is a rest.
                continue;
            }
            if (beats[i].size() > 1) {
                encodedString.push_back('[');
            }
            for(vector<char> v: beats[i]){ // iterate over the note(s) in a beat
                char code = encodeNote(v[2], v[0], v[1]); // ocatave, note letter, accidental sign
                if(code == ' ') { // check for bad beat
                    badBeat = i + 1; // make index human-friendly.
                    return 2;   // instruction is not changed in this case.
                } else {
                    encodedString.push_back(code); // playable note, store in temporary variable.
                }
            }
            if (beats[i].size() > 1) {
                encodedString.push_back(']');
            }
        }
        instructions = encodedString; // change instruction only when we are sure there is no badBeat.
        return 0;
    }
}

// give a string, return true if the string is a valid song string, return false if otherwise.

bool hasCorrectSyntax(string song) {
    vector<vector<vector<char>>> beats;
    return parseSongString(song, beats);
}

// given a string and a three dimensional vector of chars, returns true if the string is made up of zero or more beats,
// each terminated by a slash and cnsists zero or more notes. returns false if otherwise. if true,
// stores data in the three dimensional vecotor of chars, where the first index iterates over the beats in a song string,
// the second indesx iterates over the notes in a beat, and the third index indicates the attributes of a note with
// 0, 1, 2 corresponding to note letter, accidental sign, and ocatave, respectively.
// Caveat: the function always changes the value of the supplied vector, even if it returns false

bool parseSongString(const string& candidate, vector<vector<vector<char>>>& beats) {
    beats.clear();
    if (candidate.size() != 0 && candidate.at(candidate.size() - 1) != '/') {
        return false;
    } else {
        istringstream iss(candidate);
        string temp;
        vector<vector<char>> beat;
        while (iss.good()) {
            getline(iss, temp, '/');
            if (parseBeatString(temp, beat)) {
                beats.push_back(beat);
            } else {
                return false;
            }
        }
        beats.pop_back(); // getline will read in an extra empty string, discard it.
        return true;
    }
}
// given a string and a two dimensional vector of chars, return true if the string consists of zero or more notes,
// return false if otherwise. if true, the function also extracts the notes, each represented by a vector of chars,
// into a vector.
// Caveat: the function always changes the value of the supplied vector, even if it returns false

bool parseBeatString(const string& candidate, vector<vector<char>>& beat) {
    beat.clear();
    
    int index = 0;
    int length = 1;
    
    vector<char> note;
    
    while ( parseNoteString( candidate.substr( index, 1 ) , note )  ) {
        vector<char> tempNote;
        while ( parseNoteString( candidate.substr(index, length) , note ) && ( (index + length) <= candidate.size() ) ) {
            tempNote = note;
            length++;
        }
        length--;
        beat.push_back(tempNote);
        index += length;
        length = 1;
    }
    if(index == candidate.size()){
        return true;
    } else {
        return false;
    }
}

// Given a string and a vector of chars, return false if the string is not a music note
// And return true if the string is a music note and stores its note letter, accidental sign,
// and octave to the vector, in this order.
// Caveat: the function always changes the value of the supplied vector, even if it returns false
// Caveat 2: the default value of accidental sign is ' ' and that of ocatave is 4

bool parseNoteString(const string& candidate, vector<char>& note) {
    note.clear();
    switch (candidate.size()) {
        case 1:
            if (isAllowedLetter(candidate.at(0))) {
                note.push_back(candidate.at(0));
                note.push_back(' ');
                note.push_back(4);
                return true;
            } else {
                return false;
            }
            break;
        case 2:
            if ( isAllowedLetter( candidate.at(0) ) &&  isAllowedDigit( candidate.at(1) ) ) {
                note.push_back(candidate.at(0));
                note.push_back(' ');
                note.push_back(candidate.at(1) - '0');
                return true;
            } else if ( isAllowedLetter( candidate.at(0) ) && isAccidental( candidate.at(1) ) ) {
                note.push_back(candidate.at(0));
                note.push_back(candidate.at(1));
                note.push_back(4);
                return true;
            } else {
                return false;
            }
            break;
        case 3:
            if ( isAllowedLetter( candidate.at(0) ) && isAccidental( candidate.at(1) ) && isAllowedDigit( candidate.at(2) ) ) {
                note.push_back(candidate.at(0));
                note.push_back(candidate.at(1));
                note.push_back(candidate.at(2) - '0'); // since the c++ standard guarantees that the encoding of digits are continuous and increasing, subtracting a digit character by '0' yields the numerical value of the digit itself.
                return true;
            } else {
                return false;
            }
            break;
        default:
            return false;
            break;
    }
}

bool isAllowedLetter(const char& candidate){
    vector<char>allowedLetters = {'A', 'B', 'C', 'D', 'E', 'F', 'G'};
    for (int i = 0; i < allowedLetters.size(); i++) {
        if (candidate == allowedLetters[i]) {
            return true;
        }
    }
    return false;
}

bool isAllowedDigit(const char& candidate){
    return isdigit(candidate);
}

bool isAccidental(const char& candidate){
    return ( ( candidate == '#' ) || ( candidate == 'b' ) );
}



//*************************************
//  encodeNote
//*************************************

// Given an octave number, a note letter, and an accidental sign, return
// the character that the indicated note is encoded as if it is playable.
// Return a space character if it is not playable.
//
// First parameter:   the octave number (the integer 4 is the number of the
//                    octave that starts with middle C, for example).
// Second parameter:  an upper case note letter, 'A' through 'G'
// Third parameter:   '#', 'b', or ' ' (meaning no accidental sign)
//
// Examples:  encodeNote(4, 'A', ' ') returns 'Q'
//            encodeNote(4, 'A', '#') returns '%'
//            encodeNote(4, 'H', ' ') returns ' '

char encodeNote(int octave, char noteLetter, char accidentalSign)
{
    // This check is here solely to report a common CS 31 student error.
    if (octave > 9)
    {
        cerr << "********** encodeNote was called with first argument = "
        << octave << endl;
    }
    
    // Convert Cb, C, C#/Db, D, D#/Eb, ..., B, B#
    //      to -1, 0,   1,   2,   3, ...,  11, 12
    
    int note;
    switch (noteLetter)
    {
        case 'C':  note =  0; break;
        case 'D':  note =  2; break;
        case 'E':  note =  4; break;
        case 'F':  note =  5; break;
        case 'G':  note =  7; break;
        case 'A':  note =  9; break;
        case 'B':  note = 11; break;
        default:   return ' ';
    }
    switch (accidentalSign)
    {
        case '#':  note++; break;
        case 'b':  note--; break;
        case ' ':  break;
        default:   return ' ';
    }
    
    // Convert ..., A#1, B1, C2, C#2, D2, ... to
    //         ..., -2,  -1, 0,   1,  2, ...
    
    int sequenceNumber = 12 * (octave - 2) + note;
    
    string keymap = "Z1X2CV3B4N5M,6.7/A8S9D0FG!H@JK#L$Q%WE^R&TY*U(I)OP";
    if (sequenceNumber < 0  ||  sequenceNumber >= keymap.size())
        return ' ';
    return keymap[sequenceNumber];
}

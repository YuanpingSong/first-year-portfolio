//
//  Test Cases.cpp
//  Project_3
//
//  Created by Yuanping Song on 10/25/17.
//  Copyright © 2017 Yuanping Song. All rights reserved.
//

int main(int argc, const char * argv[]) {
    cout << isLegalNoteString("A") << endl; // a note letter
    cout << isLegalNoteString("H") << endl; // non valid note letter
    cout << isLegalNoteString("A3") << endl; // a note letter immediately followed by a digit
    cout << isLegalNoteString("Ab") << endl; // a note letter immediately followed by an accidental
    cout << isLegalNoteString("AA") << endl; // a note letter followed by something else
    cout << isLegalNoteString("C#3") << endl; // a note letter immediateluy followed by an accidental sign immediately followed by a digit.
    cout << isLegalNoteString("Db0") << endl; // *2
    cout << isLegalNoteString("E#9") << endl; // *3
    cout << isLegalNoteString("A%3") << endl; // invalide note
    cout << isLegalNoteString("C#0.5") << endl; // *2
    std::cout << "Hello, World!\n";
    return 0;
}


int main2(int argc, const char * argv[]) {
    string candidate = "A3";
    int octave;
    char noteLetter, accidentalSign;
    
    cout << parseNoteString(candidate, octave, noteLetter, accidentalSign) << endl;
    cout << octave << endl;
    cout << noteLetter << endl;
    cout << accidentalSign << endl;
    
    candidate = "C#5";
    
    cout << parseNoteString(candidate, octave, noteLetter, accidentalSign) << endl;
    cout << octave << endl;
    cout << noteLetter << endl;
    cout << accidentalSign << endl;
    
    candidate = "Gb0";
    
    cout << parseNoteString(candidate, octave, noteLetter, accidentalSign) << endl;
    cout << octave << endl;
    cout << noteLetter << endl;
    cout << accidentalSign << endl;
    
    candidate = "H#2";
    
    cout << parseNoteString(candidate, octave, noteLetter, accidentalSign) << endl;
    cout << octave << endl;
    cout << noteLetter << endl;
    cout << accidentalSign << endl;
    
    
    std::cout << "Hello, World!\n";
    return 0;
}


cout << isLegalBeatString("G");
cout << isLegalBeatString("A3C#E");
cout << isLegalBeatString("");
cout << isLegalBeatString("E");
cout << isLegalBeatString("F#3A3D4");
cout << isLegalBeatString("A3C#E");
cout << isLegalBeatString("C0C0DC0DD");
cout << isLegalBeatString("E#FbB#Cb");
cout << isLegalBeatString("B#9");


vector<string> notes;
string candidate = "E#FbB#Cb";
parseBeatString(candidate, notes);
for (string s: notes) {
    cout << s << endl;
}

int main(int argc, const char * argv[]) {
    
    vector<string> beats;
    string candidate = "A3C#E//E//F#3A3D4/A3C#E/";
    
    if (parseSongString(candidate, beats)) {
        for (string s: beats) {
            cout << s << endl;
        }
    } else {
        cout << "Parsing failed.";
    }
    
    std::cout << "Hello, World!\n";
    return 0;
}

bool parseNoteString(const string& candidate, int& octave, char& noteLetter, char& accidentalSign){
    
    switch (candidate.size()) {
        case 1:
            if (isAllowedLetter(candidate.at(0))) {
                noteLetter = candidate.at(0);
                octave = 4;
                accidentalSign = ' ';
                return true;
            } else {
                return false;
            }
            break;
        case 2:
            if ( isAllowedLetter( candidate.at(0) ) &&  isAllowedDigit( candidate.at(1) ) ) {
                noteLetter = candidate.at(0);
                octave = candidate.at(1) - '0';
                accidentalSign = ' ';
                return true;
            } else if ( isAllowedLetter( candidate.at(0) ) && isAccidental( candidate.at(1) ) ) {
                noteLetter = candidate.at(0);
                accidentalSign = candidate.at(1);
                octave = 4;
                return true;
            } else {
                return false;
            }
            break;
        case 3:
            if (( isAllowedLetter( candidate.at(0) ) && isAccidental( candidate.at(1) ) && isAllowedDigit( candidate.at(2) ) )) {
                octave = candidate.at(2) - '0';
                noteLetter = candidate.at(0);
                accidentalSign = candidate.at(1);
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


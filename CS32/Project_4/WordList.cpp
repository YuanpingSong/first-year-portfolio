#include "provided.h"
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <fstream>
#include <cctype>
#include <algorithm>
#include "MyHash.h"
using namespace std;

class WordListImpl
{
public:
    
    bool loadWordList(string filename);
    bool contains(string word) const;
    vector<string> findCandidates(string cipherWord, string currTranslation) const;
private:
    string patternRecognition(string word) const;
    MyHash<string, vector<string> > m_pattern_table;
    bool isWord(string s) const;
    bool isTranslation(string s) const;
    bool caseInsensitiveEqualityCheck(string s1, string s2) const;
    bool correspondenceCheck(string candidate, string currTranslation) const;
};


bool WordListImpl::loadWordList(string filename) // check complexity for this method
{
    m_pattern_table.reset();
    ifstream infile(filename);
    if(!infile) {
        return false;
    } else {
        string temp;
        while (infile >> temp ) { // check if this reads every line
            if (!isWord(temp)) {
                continue;
            } else {
                string pattern = patternRecognition(temp);
                vector<string>* vp = m_pattern_table.find(pattern);
                if (vp != nullptr) {
                    if (find(vp -> begin(), vp -> end(), temp) != vp -> end()) {
                        continue;
                    }
                    vp ->push_back(temp);
                } else {
                    vector<string> v;
                    v.push_back(temp);
                    m_pattern_table.associate(patternRecognition(temp), v);
                }
            }
        }
        return true;
    }
    
}

bool WordListImpl::contains(string word) const
{
    const vector<string> * vp = m_pattern_table.find(patternRecognition(word));
    if (vp == nullptr) {
        return false;
    } else {
        for (string s : *vp) {
            if (caseInsensitiveEqualityCheck(s, word)) {
                return true;
            }
        }
    }
    return false;
}

vector<string> WordListImpl::findCandidates(string cipherWord, string currTranslation) const
{
    vector<string> temp;
    const vector<string> * vp = m_pattern_table.find(patternRecognition(cipherWord));
    if ( !isWord(cipherWord) || !isTranslation(currTranslation) || cipherWord.size() != currTranslation.size() ||  vp == nullptr) {
        return temp;
    }
    string targetPattern = patternRecognition(cipherWord);
    for (string candidate : *vp) {
        if ( patternRecognition(candidate) == targetPattern && correspondenceCheck(candidate, currTranslation) ) { // another check is necessary due to possible collision
            temp.push_back(candidate);
        }
    }
    return temp;
}

string WordListImpl::patternRecognition(string word) const {
    for (char &c : word) {
        c = tolower(c);
    }
    string ans;
    char marker = 'A';
    string symbols;
    for (int i = 0; i < word.length(); i++) {
        if (word[i] == '\'') {
            ans += string(1,'\'');
            continue;
        }
        if (symbols.find(word[i]) == string::npos) {
            symbols.append(string(1, word[i]));
            ans += marker;
            marker++;
        } else {
            ans += ('A' + symbols.find(word[i]));
        }
    }
    return ans;
}

bool WordListImpl::isWord(string s) const {
    if (s.empty()) { // no need to keep track of empty strings
        return false;
    }
    for (char c: s) { // make sure the string is made only of letters and apostrophes.
        if ( !isalpha(c) && c != '\'') {
            return false;
        }
    }
    return true;
}

bool WordListImpl::isTranslation(string s) const {
    if (s.empty()) { // no need to keep track of empty strings
        return false;
    }
    for (char c: s) { // make sure the string is made only of letters and apostrophes.
        if ( !isalpha(c) && c != '\'' && c != '?') {
            return false;
        }
    }
    return true;
}

bool WordListImpl::caseInsensitiveEqualityCheck(string s1, string s2) const {
    for (char &c : s1) {
        c = tolower(c);
    }
    for (char &c : s2) {
        c = tolower(c);
    }
    return s1 == s2;
}

bool WordListImpl::correspondenceCheck(string candidate, string currTranslation) const {
    if (candidate.length() != currTranslation.length()) {
        return false;
    }
    for (int i = 0 ; i < candidate.length(); i++) {
        if (currTranslation[i] == '?' ) {
            if (!isalpha(candidate[i])){
                return false;
            }
        } else if (currTranslation[i] == '\''){
            if (candidate[i] != '\'') {
                return false;
            }
        } else if(isalpha(currTranslation[i]) ) {
            if (tolower(currTranslation[i]) != tolower(candidate[i])) {
                return false;
            }
        }
    }
    return true;
}


//***** hash functions for string, int, and char *****

unsigned int hash(const std::string& s)
{
    return std::hash<std::string>()(s);
}

unsigned int hash(const int& i)
{
    return std::hash<int>()(i);
}

unsigned int hash(const char& c)
{
    return std::hash<char>()(c);
}

//******************** WordList functions ************************************

// These functions simply delegate to WordListImpl's functions.
// You probably don't want to change any of this code.

WordList::WordList()
{
    m_impl = new WordListImpl;
}

WordList::~WordList()
{
    delete m_impl;
}

bool WordList::loadWordList(string filename)
{
    return m_impl->loadWordList(filename);
}

bool WordList::contains(string word) const
{
    return m_impl->contains(word);
}

vector<string> WordList::findCandidates(string cipherWord, string currTranslation) const
{
   return m_impl->findCandidates(cipherWord, currTranslation);
}

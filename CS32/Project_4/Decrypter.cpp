#include "provided.h"
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

class DecrypterImpl
{
public:
    DecrypterImpl();
    bool load(string filename);
    vector<string> crack(const string& ciphertext);
private:
    WordList m_wordList;
    Tokenizer m_tokenizer;
    Translator m_translator;
    void crackHelper(const string& ciphertext, vector<string>& ans);
    int cipherWordPicker(const vector<string>& tokens, const vector<bool>& choice) const;
    int countUnknown(const string token) const;
    int countQuestionMarks(string s);
};

DecrypterImpl::DecrypterImpl() : m_tokenizer("1234567890 ,;:.!()[]{}-\"#$%^&") {
    
}

bool DecrypterImpl::load(string filename)
{
    return m_wordList.loadWordList(filename);
}

vector<string> DecrypterImpl::crack(const string& ciphertext)
{
    vector<string> ans;
    
    crackHelper(ciphertext, ans);
    std::sort(ans.begin(), ans.end());
    return ans;  // This compiles, but may not be correct
}

/*void DecrypterImpl::crackHelper(const string& ciphertext, vector<string>& tokens, vector<bool> choices, vector<string>& ans) {
    tokens = m_tokenizer.tokenize(ciphertext);
    int idx = cipherWordPicker(tokens, choices);
    
    if (idx < 0) {
        return;
    }
    
    string pivot = tokens[idx];
    choices[idx] = true;
    
    string currentHypothesis = m_translator.getTranslation(pivot);
    vector<string> possibleMaps = m_wordList.findCandidates(pivot, currentHypothesis);
    
    for (string proposedMap: possibleMaps) {
        
        if(!m_translator.pushMapping(pivot, proposedMap)){
            continue;
        }
        string partialResult = m_translator.getTranslation(ciphertext);
        vector<string> tokenizedPartialResult = m_tokenizer.tokenize(partialResult);
        bool isPromising = true;
    
        int completedWordsCounter = 0;
        for (string token: tokenizedPartialResult) {
        
            if(token.find('?') == string::npos) {
                
                completedWordsCounter++;
            
                if ( !m_wordList.contains(token) ) {
                    m_translator.popMapping();
                    isPromising = false;
                    break;
                }
            }
        }
        if (!isPromising) {
            m_translator.popMapping();
            continue;
        }
        if (completedWordsCounter == tokens.size()) {
            ans.push_back(partialResult); // this result is no longer partial
            m_translator.popMapping();
            continue;
        }
        crackHelper(ciphertext, tokens, choices, ans);
        m_translator.popMapping();
    }
}*/

int DecrypterImpl::countQuestionMarks(string s) {
    int counter = 0;
    for (char c : s) {
        if(c == '?') {
            counter++;
        }
    }
    return counter;
}

void DecrypterImpl::crackHelper(const string& ciphertext, vector<string>& ans) {
    vector<string> tokens = m_tokenizer.tokenize(ciphertext);
    vector<string> translations = m_tokenizer.tokenize(m_translator.getTranslation(ciphertext));
    int maxIdx = -1 , maxCount = -1;
    for (int i = 0; i < translations.size(); i++) {
        int c = countQuestionMarks(translations[i]);
        if ( c > maxCount) {
            maxIdx = i;
            maxCount = c;
        }
    }
    if (maxIdx < 0) {
        return;
    }
    string pivot = tokens[maxIdx];
    vector<string> collection = m_wordList.findCandidates(pivot, m_translator.getTranslation(pivot));
    
    for (string m : collection) {
        if (!m_translator.pushMapping(pivot, m)) {
            continue;
        }
        
        int completedWordsCounter = 0;
        bool isPromising = true;
        translations = m_tokenizer.tokenize(m_translator.getTranslation(ciphertext));
        for (string token: translations) {
            if(token.find('?') == string::npos) {
                completedWordsCounter++;
                if ( !m_wordList.contains(token) ) {
                    m_translator.popMapping();
                    isPromising = false;
                    break;
                }
            }
        }
        if (!isPromising) {
            continue;
        }
        
        if (completedWordsCounter == tokens.size()) {
            ans.push_back(m_translator.getTranslation(ciphertext)); // this result is no longer partial
            m_translator.popMapping();
            continue;
        }
        crackHelper(ciphertext, ans);
        m_translator.popMapping();
    }
}




int DecrypterImpl::cipherWordPicker(const vector<string>& tokens, const vector<bool>& choice) const {
    int maxIdx = -1;
    int maxNLetters = -1;
    for (int i = 0; i < tokens.size(); i++) {
        if (choice[i]) {
            continue;
        }
        if (countUnknown(tokens[i]) > maxNLetters) {
            maxNLetters = countUnknown(tokens[i]);
            maxIdx = i;
        }
    }
    return maxIdx;
}

int DecrypterImpl::countUnknown(const string token) const {
    int counter = 0;
    for (char c : token) {
        if (m_translator.getTranslation( string(1,c) ) == string(1,'c') ) {
            counter++;
        }
    }
    return counter;
}

//******************** Decrypter functions ************************************

// These functions simply delegate to DecrypterImpl's functions.
// You probably don't want to change any of this code.

Decrypter::Decrypter()
{
    m_impl = new DecrypterImpl;
}

Decrypter::~Decrypter()
{
    delete m_impl;
}

bool Decrypter::load(string filename)
{
    return m_impl->load(filename);
}

vector<string> Decrypter::crack(const string& ciphertext)
{
   return m_impl->crack(ciphertext);
}

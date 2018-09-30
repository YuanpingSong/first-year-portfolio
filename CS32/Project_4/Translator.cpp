#include "provided.h"
#include "MyHash.h"
#include <string>
#include <vector>
using namespace std;

class TranslatorImpl
{
public:
    TranslatorImpl();
    ~TranslatorImpl();
    bool pushMapping(string ciphertext, string plaintext);
    bool popMapping();
    string getTranslation(const string& ciphertext) const;
private:
    vector<MyHash<char, char>*> m_maps;
    int m_nMaps;
    bool isLetterString(string s) const;
};

TranslatorImpl::TranslatorImpl() {
    m_nMaps = 1;
    
    MyHash<char, char> *map = new MyHash<char, char>();
    for (int i = 'a'; i <= 'z'; i++) {
        map -> associate(i, '?');
    }
    m_maps.push_back(map);
}

TranslatorImpl::~TranslatorImpl() {
    for (auto p : m_maps) {
        delete p;
    }
}


bool TranslatorImpl::pushMapping(string ciphertext, string plaintext)
{
    for(char &c : ciphertext) {
        c = tolower(c);
    }
    for(char &c : plaintext) {
        c = tolower(c);
    }
    if (ciphertext.length() != plaintext.length() || !isLetterString(ciphertext) || !isLetterString(plaintext)) {
        return false;
    }
    MyHash<char, char>* newMap = new MyHash<char, char>();
    MyHash<char, char>* inverseMap = new MyHash<char, char>();
    for (int i = 0; i < 26; i++) {
        char cypher = 'a' + i;
        char plain = *(m_maps[m_nMaps - 1] -> find('a' + i));
        newMap -> associate(cypher, plain);
        inverseMap -> associate(plain, cypher);
    }
    
    for (int i = 0; i < ciphertext.length(); i++) {
        char oldTarget = *(newMap -> find(ciphertext[i]));
        char* oldSourcePtr = inverseMap -> find(plaintext[i]);
        bool condition1 = oldTarget == '?' || oldTarget == plaintext[i]; // disallow overwriting mappings
        bool condition2 = oldSourcePtr == nullptr || *oldSourcePtr == ciphertext[i];
        if ( !condition1 || !condition2  ) {
            return false;
        } else {
            newMap -> associate(ciphertext[i], plaintext[i]);
            inverseMap -> associate(plaintext[i], ciphertext[i]);
        }
    }
    delete inverseMap;
    m_maps.push_back(newMap);
    m_nMaps++;
    return true;
}

bool TranslatorImpl::popMapping()
{
    if (m_nMaps > 1) {
        m_nMaps--;
        m_maps.pop_back();
        return true;
    }
    return false;
}

string TranslatorImpl::getTranslation(const string& ciphertext) const
{
    string ans;
    for (char c : ciphertext) {
        if (!isalpha(c)) {
            ans += string(1,c);
            continue;
        }
        char map;
        if (isupper(c)) {
            map = toupper( *m_maps[m_nMaps - 1] -> find( tolower(c) ));
        } else {
            map = *m_maps[m_nMaps - 1] -> find( tolower(c) );
        }
        ans += string(1, map);
    }
    return ans;
}

bool TranslatorImpl::isLetterString(string s) const {
    for(char c : s) {
        if(!isalpha(c)) {
            return false;
        }
    }
    return true;
}

//******************** Translator functions ************************************

// These functions simply delegate to TranslatorImpl's functions.
// You probably don't want to change any of this code.

Translator::Translator()
{
    m_impl = new TranslatorImpl;
}

Translator::~Translator()
{
    delete m_impl;
}

bool Translator::pushMapping(string ciphertext, string plaintext)
{
    return m_impl->pushMapping(ciphertext, plaintext);
}

bool Translator::popMapping()
{
    return m_impl->popMapping();
}

string Translator::getTranslation(const string& ciphertext) const
{
    return m_impl->getTranslation(ciphertext);
}

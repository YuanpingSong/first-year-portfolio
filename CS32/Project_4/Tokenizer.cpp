#include "provided.h"
#include <string>
#include <vector>

using namespace std;

class TokenizerImpl
{
public:
    TokenizerImpl(string separators);
    vector<string> tokenize(const string& s) const;
private:
    string m_seperators;
};

TokenizerImpl::TokenizerImpl(string separators) : m_seperators(separators)
{
    
}

vector<string> TokenizerImpl::tokenize(const string& s) const
{
    vector<string> ans;
    string token;
    for (char c : s) {
        if (m_seperators.find(c) == string::npos) {
            token += c;
        } else if (!token.empty()) {
            ans.push_back(token);
            token = "";
        }
    }
    if (!token.empty()) {
        ans.push_back(token);
    }
    return ans;
    
}

//******************** Tokenizer functions ************************************

// These functions simply delegate to TokenizerImpl's functions.
// You probably don't want to change any of this code.

Tokenizer::Tokenizer(string separators)
{
    m_impl = new TokenizerImpl(separators);
}

Tokenizer::~Tokenizer()
{
    delete m_impl;
}

vector<string> Tokenizer::tokenize(const string& s) const
{
    return m_impl->tokenize(s);
}

//
//  main.cpp
//  CS32_HW_3
//
//  Created by Yuanping Song on 2/12/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

/*#include <iostream>
#include <string>
using namespace std;*/

class Investment {
public:
    Investment(string &name, int amount) : m_name(name), m_price(amount) {
        
    }
    virtual ~Investment() {
        
    }
    
    virtual string description() const = 0;
    virtual bool fungible() const {
        return false;
    }
    string name() const { return m_name; }
    int purchasePrice() const { return m_price; }
private:
    string m_name;
    int m_price;
};

class Painting : public Investment {
public:
    Painting(string name, int amount) : Investment(name, amount) {
        
    }
    virtual ~Painting() {
        cout << "Destroying " << name() << ", a painting" << endl;
    }
    virtual string description() const {
        return "painting";
    }
    
};

class Stock : public Investment {
public:
    Stock(string name, int amount, string symbol) : Investment(name, amount), m_symbol(symbol) {
        
    }
    virtual ~Stock() {
        cout << "Destroying " << name() << ", a stock holding" << endl;
    }
    virtual string description() const {
        return "stock trading as " + m_symbol;
    }
    virtual bool fungible() const {
        return true;
    }
private:
    string m_symbol;
};

class House : public Investment {
public:
    House(string address, int amount) : Investment(address, amount) {
        
    }
    virtual ~House() {
        cout << "Destroying the house " << name() << endl;
    }
    virtual string description() const {
        return "house";
    }
};

/*void display(const Investment* inv)
{
    cout << inv->name();
    if (inv->fungible())
        cout << " (fungible)";
    cout << ": " << inv->description() << " bought for $" << inv->purchasePrice() << endl;
}

int main()
{
    Investment* portfolio[4];
    portfolio[0] = new Painting("Salvator Mundi", 450300000);
    // Stock holdings have a name, value, and ticker symbol
    portfolio[1] = new Stock("Alphabet", 100000, "GOOGL");
    portfolio[2] = new Stock("Symantec", 50000, "SYMC");
    portfolio[3] = new House("4 Privet Drive", 660000);
    
    for (int k = 0; k < 4; k++)
        display(portfolio[k]);
    
    // Clean up the investments before exiting
    cout << "Cleaning up" << endl;
    for (int k = 0; k < 4; k++)
        delete portfolio[k];
}*/


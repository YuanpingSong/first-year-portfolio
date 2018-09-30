//
//  Map.h
//  Project_2
//
//  Created by Yuanping Song on 1/28/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#ifndef Map_h
#define Map_h

#include <string>

using KeyType = std::string;
using ValueType = double;

const int ELEMENT_NOT_FOUND = -1;

class Map
{
public:
    Map();
    ~Map(); // destructor, deallocate nodes
    Map(const Map &src); // copy constructor, dynamically allocate new nodes
    Map& operator= (const Map &src); // assignment operator, deallocate nodes in target, dynamically copy nodes from source
    bool empty() const { return head == nullptr; }
    int size() const;
    bool insert(const KeyType& key, const ValueType& value);
    bool update(const KeyType& key, const ValueType& value);
    bool insertOrUpdate(const KeyType& key, const ValueType& value);
    bool erase(const KeyType& key);
    bool contains(const KeyType& key) const;
    bool get(const KeyType& key, ValueType& value) const;
    bool get(int i, KeyType& key, ValueType& value) const;
    void swap(Map& other);
    void dump() const; // write to cerr not cout VERY IMPORTANT!!!
    
    // these are not part of the interface, move them into private section after testing
    
private:
    // Helper Functions
    int indexOf(const KeyType& key) const;
    bool insertIntoEmptyMap(const KeyType& key, const ValueType &val);
    bool insertAtBack(const KeyType& key, const ValueType &val);
    bool insertAtFront(const KeyType& key, const ValueType &val);
    bool eraseOnly();
    bool eraseAtFront();
    bool eraseAtBack();
    
    struct Node{
        KeyType key;
        ValueType value;
        Node *prev, *next;
        Node() { } // default constructor;
        Node(KeyType k, ValueType v) : key(k), value(v) { }
        Node(KeyType k, ValueType v, Node *p, Node *n) : key(k), value(v), prev(p), next(n) { }
    };
    
    Node* head;
    Node* tail;
};

bool combine(const Map& m1, const Map& m2, Map& result);
void subtract(const Map& m1, const Map& m2, Map& result);


#endif /* Map_h */

//
//  Map.cpp
//  Project_2
//
//  Created by Yuanping Song on 1/28/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include "Map.h"
#include <iostream>

Map::Map() : head(nullptr), tail(nullptr) {
    
}

Map::~Map() {
    Node *curr = head;
    while (curr != nullptr) {
        Node *np = curr -> next;
        delete curr;
        curr = np;
    }
}

Map::Map(const Map &src) {
    *this = src;
}

Map& Map::operator=(const Map &src) {
    if (&src == this) {
        // self assignment, do nothing
    } else {
        // deallocate memory associated with current obj.
        if (head == nullptr && tail == nullptr) {
            // do nothing
        } else {
            /*Node *curr = head;
            while (curr != nullptr) {
                Node *np = curr -> next;
                delete curr;
                curr = np;
            }*/
            while (size() > 0) {
                KeyType key;
                ValueType value;
                get(1, key, value );
                erase(key);
            }
        }
        // create nodes in stack 
        for (int i = 0; i < src.size(); i++) {
            KeyType key;
            ValueType val;
            src.get(i, key, val);
            insert(key, val);
        }
    }
    return *this;
}

int Map::size() const {
    if (head == nullptr) {
        return 0;
    } else {
        int count = 1;
        Node* it = head;
        while (it != tail) {
            it= it -> next;
            count++;
        }
        return count;
    }
}

bool Map::insert(const KeyType& key, const ValueType& value) {
    if (contains(key)) {
        return false;
    } else if (head == nullptr) {
        return insertIntoEmptyMap(key, value);
    } else {
        Node *it = head;
        while (it != nullptr) {
            if (key < it -> key ) {
                break;
            }
            it = it -> next;
        }
        if (it == head) {
            return insertAtFront(key, value);
        } else if (it == nullptr) {
            return insertAtBack(key, value);
        } else {
            Node *p = new Node(key, value, it -> prev, it);
            it -> prev -> next = p;
            it -> prev = p;
            return true;
        }
    }
}

bool Map::update(const KeyType& key, const ValueType& value) {
    int idx = indexOf(key);
    if (idx == ELEMENT_NOT_FOUND) {
        return false;
    } else {
        Node *it = head;
        for (int i = 0; i < idx; i++) {
            it = it -> next;
        }
        it -> value = value;
        return true;
    }
}

bool Map::insertOrUpdate(const KeyType& key, const ValueType& value) {
    if (indexOf(key) == ELEMENT_NOT_FOUND) {
        return insert(key, value);
    } else {
        return update(key, value); // should always return true
    }
}

bool Map::erase(const KeyType& key) {
    
    int determinant = indexOf(key);
    if ( determinant == ELEMENT_NOT_FOUND) {
        return false;
    } else if (determinant == 0){
        return eraseAtFront();
    } else {
        Node *it = head;
        for (int i = 0; i < determinant; i++) {
            it = it -> next;
        }
        if (tail == it) {
            return eraseAtBack();
        }
        it -> prev -> next = it -> next;
        it -> next -> prev = it -> prev;
        delete it;
        return true;
    }
}

bool Map::contains(const KeyType& key) const {
    
    return indexOf(key) != ELEMENT_NOT_FOUND;
}

bool Map::get(const KeyType& key, ValueType& value) const {
    int determinant = indexOf(key);
    if (determinant == ELEMENT_NOT_FOUND) {
        return false;
    } else {
        KeyType tempKey;
        return this -> get(determinant, tempKey, value);
    }
}

bool Map::get(int i, KeyType& key, ValueType& value) const {
    if (i < 0 || i >= size()) {
        return false;
    } else {
        Node *it = head;
        for(int j = 0; j < i; j++) {
            it = it -> next;
        }
        key = it -> key;
        value = it -> value;
        return true;
    }
}

void Map::swap(Map& other) { // no need to swap dynamically allocated nodes. just switch head and tail pointers.
    Node *tempHead, *tempTail;
    tempHead = head;
    tempTail = tail;
    head = other.head;
    tail = other.tail;
    other.head = tempHead;
    other.tail = tempTail;
}

void Map::dump() const {
    Node *it = head;
    while (it != nullptr) {
        std::cerr << "key: " << it -> key << ", value: " << it -> value << std::endl;
        it = it -> next;
    }
    std::cerr << "The size of the Map is: " << size() << std::endl;
}

int Map::indexOf(const KeyType& key) const {
    Node *it = head;
    int idx = 0;
    while (it != nullptr) { // loop invariant: the element sought is not in the range [head, it).
        if (it -> key == key) {
            return idx;
        }
        it = it -> next;
        idx++;
    } // upon exiting this loop, it points toward the position after the last node. the loop invariant ensures that key is not in the range [head, tail], so the function returns false.
    return ELEMENT_NOT_FOUND;
}

bool Map::insertIntoEmptyMap(const KeyType& key, const ValueType &val) {
    if (head == nullptr && tail == nullptr) {
        Node *p = new Node(key, val, head, tail);
        head = p;
        tail = p;
        return true;
    } else {
        return false;
    }
}

bool Map::insertAtBack(const KeyType& key, const ValueType &val) {
    if (tail != nullptr) {
        Node *p = new Node(key, val, tail, nullptr);
        tail -> next = p;
        tail = p;
        return true;
    } else {
        return insertIntoEmptyMap(key, val);
    }
}

bool Map::insertAtFront(const KeyType& key, const ValueType &val) {
    if (head != nullptr) {
        Node *p = new Node(key, val, nullptr, head);
        head -> prev = p;
        head = p;
        return true;
    } else {
        return insertIntoEmptyMap(key, val);
    }
}

bool Map::eraseOnly() {
    if (head != nullptr && tail != nullptr && head == tail) {
        delete head;
        head = nullptr;
        tail = nullptr;
        return true;
    } else {
        return false;
    }
}

bool Map::eraseAtFront() {
    if (head == nullptr) {
        return false;
    } else if (head == tail) {
        return eraseOnly();
    } else {
        Node *p = head;
        head = head -> next;
        head -> prev = nullptr;
        delete p;
        return true;
    }
}

bool Map::eraseAtBack() {
    if (tail == nullptr) {
        return false;
    } else if (tail == head) {
        return eraseOnly();
    } else {
        Node *p = tail;
        tail = tail -> prev;
        tail -> next = nullptr;
        delete p;
        return true;
    }
}

bool combine(const Map& m1, const Map& m2, Map& result) {
    Map ans;
    bool rtn = true;
    
    // insert all elements in m1 into ans
    for (int i = 0; i < m1.size(); i++) {
        KeyType key;
        ValueType val;
        m1.get(i,key, val);
        ans.insert(key, val);
    }
    
    // insert non-conflicting elements from m2 into ans
    for (int i = 0; i < m2.size(); i++) {
        KeyType key;
        ValueType val;
        m2.get(i,key, val);
        if(!ans.insert(key, val)){ // resolve duplication
            ValueType tempVal;
            ans.get(key, tempVal); // this gets the key value pair from m1
            if (val != tempVal) { // if there is a conflict
                rtn = false; // set return value to false
                ans.erase(key); // remove the pair from ans.
            }
        }
    }
    
    result = ans;
    return rtn;
}

void subtract(const Map& m1, const Map& m2, Map& result) {
    Map ans;
    for (int i = 0; i < m1.size(); i++) {
        KeyType tempKey;
        ValueType tempVal;
        m1.get(i, tempKey, tempVal);
        if (m2.contains(tempKey)) {
            continue;
        }
        ans.insert(tempKey, tempVal);
    }
    result = ans;
}


//
//  Map.cpp
//  HW_1
//
//  Created by Yuanping Song on 1/17/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include "Map.h"
#include <array>
#include <iostream>
#include <utility>

const int ELEMENT_NOT_FOUND = -1;

// This is a private helper function
int Map::indexOf(const KeyType& key) const {
    for(int i = 0; i < m_size; i++){
        if (m_arr[i].m_key == key) {
            return i;
        }
    }
    return ELEMENT_NOT_FOUND;
}

bool Map::contains(const KeyType& key) const {
    return indexOf(key) != ELEMENT_NOT_FOUND;
}

bool Map::insert(const KeyType& key, const ValueType& value) {
    if( contains(key) || m_size >= DEFAULT_MAX_ITEMS ){
        return false;
    } else {
        m_arr[m_size] = item(key, value);
        m_size++;
        return true;
    }
}

bool Map::update(const KeyType& key, const ValueType& value) {
    if ( contains(key) ) {
        m_arr[indexOf(key)].m_value = value;
        return true;
    } else {
        return false;
    }
}

bool Map::insertOrUpdate(const KeyType& key, const ValueType& value) {
    if ( contains(key) ) {
        return update(key, value); // this statement should always return true;
    } else {
        return insert(key, value);
    }
}

bool Map::erase(const KeyType& key) {
    if (contains(key)) {
        for (int i = indexOf(key); i + 1 < m_size; i++) {
            m_arr[i] = m_arr[i + 1];
        }
        m_size--;
        return true;
    } else {
        return false;
    }
}

bool Map::get(const KeyType& key, ValueType& value) const {
    if (contains(key)) {
        value = m_arr[indexOf(key)].m_value;
        return true;
    } else {
        return false;
    }
}

bool Map::get(int i, KeyType& key, ValueType& value) const {
    if ( 0 <= i && i < m_size ) {
        key = m_arr[i].m_key;
        value = m_arr[i].m_value;
        return true;
    } else {
        return false;
    }
}

void Map::swap(Map& other) {
    std::swap(*this, other);
}

void Map::dump() const {
    for (int i = 0; i < m_size; i++) {
        std::cerr << m_arr[i].m_key << " " <<m_arr[i].m_value << std::endl;
    }
}




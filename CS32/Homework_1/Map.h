//
//  Map.h
//  HW_1
//
//  Created by Yuanping Song on 1/17/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#ifndef Map_h
#define Map_h

#include <string>

using KeyType = std::string;
using ValueType = double;

const int DEFAULT_MAX_ITEMS = 250;

class Map
{
public:
    Map() { m_size = 0; }         // Create an empty map (i.e., one with no key/value pairs)
    
    bool empty() const { return m_size == 0; } // Return true if the map is empty, otherwise false.
    
    int size() const { return m_size; }    // Return the number of key/value pairs in the map.
    
    bool insert(const KeyType& key, const ValueType& value);
    // If key is not equal to any key currently in the map, and if the
    // key/value pair can be added to the map, then do so and return true.
    // Otherwise, make no change to the map and return false (indicating
    // that either the key is already in the map, or the map has a fixed
    // capacity and is full).
    
    bool update(const KeyType& key, const ValueType& value);
    // If key is equal to a key currently in the map, then make that key no
    // longer map to the value it currently maps to, but instead map to
    // the value of the second parameter; return true in this case.
    // Otherwise, make no change to the map and return false.
    
    bool insertOrUpdate(const KeyType& key, const ValueType& value);
    // If key is equal to a key currently in the map, then make that key no
    // longer map to the value it currently maps to, but instead map to
    // the value of the second parameter; return true in this case.
    // If key is not equal to any key currently in the map and if the
    // key/value pair can be added to the map, then do so and return true.
    // Otherwise, make no change to the map and return false (indicating
    // that the key is not already in the map and the map has a fixed
    // capacity and is full).
    
    bool erase(const KeyType& key);
    // If key is equal to a key currently in the map, remove the key/value
    // pair with that key from the map and return true.  Otherwise, make
    // no change to the map and return false.
    
    bool contains(const KeyType& key) const;
    // Return true if key is equal to a key currently in the map, otherwise
    // false.
    
    bool get(const KeyType& key, ValueType& value) const;
    // If key is equal to a key currently in the map, set value to the
    // value in the map that that key maps to, and return true.  Otherwise,
    // make no change to the value parameter of this function and return
    // false.
    
    bool get(int i, KeyType& key, ValueType& value) const;
    // If 0 <= i < size(), copy into the key and value parameters the
    // key and value of one of the key/value pairs in the map and return
    // true.  Otherwise, leave the key and value parameters unchanged and
    // return false.  (See below for details about this function.)
    
    void swap(Map& other);
    // Exchange the contents of this map with the other one.
    
    void dump() const;
    // This is a dump function for testing purposes. 
private:
    int indexOf(const KeyType& key) const;
    
    struct item {
        KeyType m_key;
        ValueType m_value;
        
        item() { } // default constructor needs to be preserved for array initialization.
        item(KeyType key, ValueType value): m_key(key), m_value(value) { }
    };
    
    item m_arr[DEFAULT_MAX_ITEMS]; // data structure, an array of items (key-value paris)
    int m_size; // counter variable for the number of slots in m_arr that is in use. also indexes the position where the next insertion could take place.
};


#endif /* Map_h */

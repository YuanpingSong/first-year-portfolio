//
//  CarMap.cpp
//  HW_1
//
//  Created by Yuanping Song on 1/17/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include "CarMap.h"
#include <iostream>

bool CarMap::addCar(KeyType license) {
    return m_map.insert(license, 0);
}

ValueType CarMap::gas(KeyType license) const {
    ValueType gas;
    if (m_map.get(license, gas)) {
        return gas;
    } else {
        return -1;
    }
}

bool CarMap::addGas(KeyType license, ValueType gallons) {
    ValueType gas = 0;
    if (!m_map.get(license, gas) || gas < 0) {
        return false;
    } else {
        m_map.update(license, gas += gallons);
        return true;
    }
}

bool CarMap::useGas(KeyType license, ValueType gallons) {
    ValueType gas = 0;
    if (!m_map.get(license, gas) || gas < 0) {
        return false;
    } else {
        m_map.update(license, gas -= gallons);
        return true;
    }
}
void CarMap::print() const {
    for (int i = 0; i < fleetSize(); i++) {
        KeyType key;
        ValueType value;
        m_map.get(i, key, value);
        std::cout << key << " " << value << std::endl;
    }
}


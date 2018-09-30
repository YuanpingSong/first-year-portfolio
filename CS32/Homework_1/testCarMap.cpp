//
//  testCarMap.cpp
//  HW_1
//
//  Created by Yuanping Song on 1/17/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//


#include "CarMap.h"
#include <cassert>
#include <iostream>
int main() {
    CarMap cm;
    cm.addCar("HDFD890");
    cm.addCar("4FS839S");
    cm.addCar("FDU8023");
    assert(cm.addGas("HDFD890", 15));
    assert(!cm.addGas("HDFD90", 15));
    assert(cm.useGas("HDFD890", 150));
    assert(!cm.useGas("HDFD890", 1));
    std::cout << cm.fleetSize();
    assert(cm.gas("74820") == -1);
    assert(cm.gas("HDFD890") == -135);
    cm.print();
    
    CarMap myFleet;
    assert(myFleet.addCar("1234567"));
    assert(myFleet.gas("1234567") == 0);
    assert(myFleet.addGas("1234567", 100));
    assert(myFleet.gas("1234567") == 100);
    myFleet.print();
    myFleet = cm;
    myFleet.print();
    
}


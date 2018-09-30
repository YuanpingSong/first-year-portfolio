//
//  main.cpp
//  Project_1
//
//  Created by Yuanping Song on 1/11/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//


#include <iostream>
#include <cstdlib>
#include "Game.h"
using namespace std;

///////////////////////////////////////////////////////////////////////////
//  main()
///////////////////////////////////////////////////////////////////////////

int main()
{
    // Create a game
    // Use this instead to create a mini-game:   Game g(3, 4, 2);
    Game g(7, 8, 25);
    
    // Play the game
    g.play();
}



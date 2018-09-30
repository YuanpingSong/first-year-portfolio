//
//  globals.h
//  Project_1
//
//  Created by Yuanping Song on 1/11/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#ifndef globals_h
#define globals_h
///////////////////////////////////////////////////////////////////////////
// Manifest constants
///////////////////////////////////////////////////////////////////////////

const int MAXROWS = 20;               // max number of rows in a city
const int MAXCOLS = 30;               // max number of columns in a city
const int MAXFLATULANS = 100;         // max number of Flatulans allowed
const int INITIAL_PLAYER_HEALTH = 10;

const int UP      = 0;
const int DOWN    = 1;
const int LEFT    = 2;
const int RIGHT   = 3;
const int NUMDIRS = 4;

int decodeDirection(char dir);
int randInt(int min, int max);
void clearScreen();


#endif /* globals_h */



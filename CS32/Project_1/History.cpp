//
//  History.cpp
//  Project_1
//
//  Created by Yuanping Song on 1/12/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include "History.h"
#include <iostream>


History::History(int nRows, int nCols) : m_rows(nRows), m_cols(nCols) {
    for(int i = 0; i < MAXROWS; i++){
        for (int j = 0; j < MAXCOLS; j++) {
            grid[i][j] = 0;
        }
    }
}

bool History::record(int r , int c) {
    if (r <= 0 || c <= 0 || r > m_rows || c > m_cols) {
        return false;
    } else {
        r--;
        c--;
        grid[r][c]++;
        return true;
    }
}

void History::display() const {
    clearScreen();
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0 ; j < m_cols; j++) {
            if (grid[i][j] == 0) {
                std::cout << '.';
            } else {
                int ascii = (grid[i][j] > 26)? 90 : grid[i][j] + 64;
                std::cout << static_cast<char>(ascii);
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

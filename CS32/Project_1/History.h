//
//  History.h
//  Project_1
//
//  Created by Yuanping Song on 1/12/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#ifndef History_h
#define History_h

#include "globals.h"

class History
{
public:
    History(int nRows, int nCols);
    bool record(int r, int c);
    void display() const;
private:
    int m_rows, m_cols;
    int grid[MAXROWS][MAXCOLS];
    
};

#endif /* History_h */

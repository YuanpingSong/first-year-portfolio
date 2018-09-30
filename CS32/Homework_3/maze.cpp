//
//  maze.cpp
//  CS32_HW_3
//
//  Created by Yuanping Song on 2/12/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//
/*#include <string>
#include <stack>
#include <iostream>
using namespace std;*/



class Coord
{
public:
    Coord(int rr, int cc) : m_r(rr), m_c(cc) {}
    int r() const { return m_r; }
    int c() const { return m_c; }
private:
    int m_r;
    int m_c;
};



/*void explore(string maze[], stack<Coord>& toDo, int r, int c)
{
    if (maze[r][c] == OPEN)
    {
        toDo.push(Coord(r,c));
        maze[r][c] = SEEN;  // anything non-OPEN will do
    }
}*/

bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec)
{
    
    const char WALL = 'X';
    const char OPEN = '.';
    const char SEEN = 'o';
    if (sr < 0  ||  sr >= nRows  ||  sc < 0  || sc >= nCols  ||
        er < 0  ||  er >= nRows  ||  ec < 0  || ec >= nCols  ||
        maze[sr][sc] != OPEN  ||  maze[er][ec] != OPEN)
        return false;
    
    if (sr == er && sc == ec) {
        return true;
    } else {
        maze[sr][sc] = SEEN;
        if (pathExists(maze, nRows, nCols, sr, sc + 1, er, ec)) { // east
            return true;
        }
        if (pathExists(maze, nRows, nCols, sr + 1, sc, er, ec)) { // south
            return true;
        }
        if (pathExists(maze, nRows, nCols, sr, sc - 1, er, ec)) { // west
            return true;
        }
        if (pathExists(maze, nRows, nCols, sr - 1, sc, er, ec)) { // north
            return true;
        }
    }
    return false;
}

/*int main()
{
    string maze[10] = {
        "XXXXXXXXXX",
        "X...X..X.X",
        "X..XX....X",
        "X.X.XXXX.X",
        "XXX......X",
        "X...X.XX.X",
        "X.X.X..X.X",
        "X.XXXX.X.X",
        "X..X...X.X",
        "XXXXXXXXXX"
    };
    
    if (pathExists(maze, 10,10, 4,3, 1,8))
        cout << "Solvable!" << endl;
    else
        cout << "Out of luck!" << endl;
}*/

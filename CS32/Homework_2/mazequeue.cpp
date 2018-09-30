//
//  mazequeue.cpp
//  CS32_HW_2
//
//  Created by Yuanping Song on 2/6/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

//
//  main.cpp
//  CS32_HW_2
//
//  Created by Yuanping Song on 2/6/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include <iostream>
#include <queue>
using namespace std;

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

// Return true if there is a path from (sr,sc) to (er,ec)
// through the maze; return false otherwise

/*
 Push the starting coordinate (sr,sc) onto the coordinate stack and
 update maze[sr][sc] to indicate that the algorithm has encountered
 it (i.e., set maze[sr][sc] to have a value other than '.').
 While the stack is not empty,
 {   Pop the top coordinate off the stack. This gives you the current
 (r,c) location that your algorithm is exploring.
 If the current (r,c) coordinate is equal to the ending coordinate,
 then we've solved the maze so return true!
 Check each place you can move from the current cell as follows:
 If you can move EAST and haven't encountered that cell yet,
 then push the coordinate (r,c+1) onto the stack and update
 maze[r][c+1] to indicate the algorithm has encountered it.
 If you can move SOUTH and haven't encountered that cell yet,
 then push the coordinate (r+1,c) onto the stack and update
 maze[r+1][c] to indicate the algorithm has encountered it.
 If you can move WEST and haven't encountered that cell yet,
 then push the coordinate (r,c-1) onto the stack and update
 maze[r][c-1] to indicate the algorithm has encountered it.
 If you can move NORTH and haven't encountered that cell yet,
 then push the coordinate (r-1,c) onto the stack and update
 maze[r-1][c] to indicate the algorithm has encountered it.
 }
 There was no solution, so return false
 */
bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec) {
    queue<Coord> cqueue;
    cqueue.push(Coord(sr, sc));
    maze[sr][sc] = 'P';
    
    int counter = 0; // code for problem 2
    
    while ( !cqueue.empty() ) {
        // dump loop
        /*for (int i = 0; i < nRows; i++) {
         cout << maze[i] << endl;
         }
         cout << endl;*/
        Coord curr = cqueue.front();
        // Code for problem 4
        /*if (counter < 12) {
         counter++;
         cout << "Coord " << counter << ": (" << curr.r() << ", " << curr.c() << ")" << endl;
         }*/
        
        cqueue.pop();
        if (curr.r() == er && curr.c() == ec) {
            return true;
        }
        if ( curr.c() + 1 < nCols && maze[curr.r()][curr.c() + 1] == '.') { // East
            cqueue.push(Coord(curr.r(), curr.c() + 1));
            maze[curr.r()][curr.c() + 1] = 'P';
        }
        if ( curr.r() + 1 < nRows && maze[curr.r() + 1][curr.c()] == '.') { // South
            cqueue.push(Coord(curr.r() + 1, curr.c()));
            maze[curr.r() + 1][curr.c()] = 'P';
        }
        if ( curr.c() - 1 >= 0 && maze[curr.r()][curr.c() - 1] == '.') { // West
            cqueue.push(Coord(curr.r(), curr.c() - 1));
            maze[curr.r()][curr.c() - 1] = 'P';
        }
        if ( curr.r() - 1 >= 0 && maze[curr.r() - 1][curr.c()] == '.') { // North
            cqueue.push(Coord(curr.r() - 1, curr.c()));
            maze[curr.r() - 1][curr.c()] = 'P';
        }
    }
    return false;
}


int main()
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
}


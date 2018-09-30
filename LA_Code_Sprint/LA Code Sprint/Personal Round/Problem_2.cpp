//
//  Problem_2.cpp
//  LA Code Sprint
//
//  Created by Yuanping Song on 5/20/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//

#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <set>
#include <map>
#include <queue>
#include <cassert>
#include <cmath>
#include <climits>

using namespace std;

int main() {
    int numLocks = 0;
    cin >> numLocks;
    while (numLocks > 0) {
        vector<vector<int>> data;
        data.push_back(vector<int>());
        data.push_back(vector<int>());
        data.push_back(vector<int>());
        int temp;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 100; j++) {
                cin >> temp;
                data[i].push_back(temp);
            }
        }
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 100; j++) {
                if (data[i][j] == 1) {
                    cout << j;
                    if (i != 2) {
                        cout << " ";
                    } else {
                        cout << endl;
                    }
                }
            }
        }
        //cin >> temp; // consume newline
        //cout << endl;
        numLocks--;
    }
}

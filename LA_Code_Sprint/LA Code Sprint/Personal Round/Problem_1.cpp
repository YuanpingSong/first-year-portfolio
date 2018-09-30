//
//  Problem_1.cpp
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
    int numTests = 0;
    cin >> numTests;
    vector<int> o1,o2;
    for (int i = 0; i < numTests; i++) {
        int numJumps = 0;
        cin >> numJumps;
        int p = 0, q = 0;
        cin >> p;
        cin >> q;
        for (int j = 0; j < p ; j++) {
            int temp;
            cin >> temp;
            o1.push_back(temp);
        }
        for (int j = 0; j < q ; j++) {
            int temp;
            cin >> temp;
            o2.push_back(temp);
        }
        sort(o1.begin(), o1.end());
        sort(o2.begin(), o2.end());
        int max = INT_MIN;
        int jumps = 0;
        for (int k = 0; k < p; k++) {
            for (int m = 0; m < q; m++) {
                jumps = o1[k] + o2[m];
                if (jumps <= numJumps && jumps > max) {
                    max = jumps;
                }
            }
        }
        cout << max << endl;
        o1.clear();
        o2.clear();
    }
    
    
}

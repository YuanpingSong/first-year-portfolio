//
//  Problem_3.cpp
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

int CountUniqueCharacters(string str){
    int count = 0;
    
    for (int i = 0; i < str.length(); i++){
        bool appears = false;
        for (int j = 0; j < i; j++){
            if (str[j] == str[i]){
                appears = true;
                break;
            }
        }

        if (!appears){
            count++;
        }
    }
    
    return count;
}

int main() {
    int numCases;
    int N, C;
    cin >> numCases;
    while (numCases > 0) {
        cin >> N;
        cin >> C;
        string answer;
        cin >> answer;
    }
}

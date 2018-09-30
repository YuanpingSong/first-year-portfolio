//
//  Problem_A.cpp
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

using namespace std;

int main() {
    long long numTests;
    cin >> numTests;
    while (numTests > 0) {
        long long numPings, numMates, numTypes; // numPings, numTeammates, numPings.
        cin >> numPings >> numMates >> numTypes;
        vector<long long> players;
        vector<long long> pings;
        for (int i = 0 ; i < numMates; i++) {
            int temp;
            cin >> temp;
            if (temp != 0) {
                players.push_back(temp);
            }
        }
        for (int i = 0 ; i < numTypes; i++) {
            int temp;
            cin >> temp;
            if (temp != 0) {
                pings.push_back(temp);
            }
        }
        sort(players.rbegin(), players.rend());
        sort(pings.rbegin(), pings.rend());
        
        long long playerSum = 0, pingSum = 0;
        for (long long x : players) {
            playerSum += x;
        }
        for (long long x : players) {
            playerSum += x;
        }
        
        long long accumulator = 0;
        bool hasRunOut = false;
        for (int i = 0; i < numPings; i++) {
            if (players.empty() || pings.empty() ) {
                cout << accumulator << endl;
                hasRunOut = true;
                break;
            }
            if (players.front() > pings.front() || ( (players.front() == pings.front()) && (players.back() > pings.back()))) {
                accumulator += players.front();
                players.front()--;
                pings.back()--;
                sort(players.rbegin(), players.rend());
                sort(pings.rbegin(), pings.rend());
                if (pings.back() == 0) {
                    pings.pop_back();
                }
                if (players.back() == 0) {
                    players.pop_back();
                }
            } else {
                accumulator += pings.front();
                pings.front()--;
                players.back()--;
                sort(players.rbegin(), players.rend());
                sort(pings.rbegin(), pings.rend());
                if (pings.back() == 0) {
                    pings.pop_back();
                }
                if (players.back() == 0) {
                    players.pop_back();
                }
            }
        }
        if (!hasRunOut) {
            cout << accumulator << endl;
        }
        numTests--;
    }
}

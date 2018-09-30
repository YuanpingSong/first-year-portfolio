//
//  Problem_4.cpp
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

// found this online....
int maxSubArray(vector<int>& nums) {
    int n = nums.size();
    if(n == 0)
        return 0;
    if(n == 1)
        return nums[0];
    int sum_cur = nums[0];
    int max_sum = nums[0];
    for(int i=1;i<n;++i){
		
        if(sum_cur < 0)
            sum_cur = nums[i];
        else
            sum_cur += nums[i];
		
        if(sum_cur > max_sum)
            max_sum = sum_cur;
    }
	
    return max_sum;
}

int main() {
    int numTC = 0;
    cin >> numTC;
    while (numTC >0) {
        int n;
        cin >> n;
        string game;
        for (int i = 0; i < pow(2, n); i++) {
            char temp;
            cin >> temp;
            game.push_back(temp);
        }
        numTC--;
    }
}

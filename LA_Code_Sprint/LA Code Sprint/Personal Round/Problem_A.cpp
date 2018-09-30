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

typedef long long ll;
typedef vector<ll> vi;

int main() {
    int n; cin >> n;
    
    vi a(n);
    ll maxx = 0;
    for(int i =0; i < n; i++) {
        cin >> a[i];
        maxx = max(maxx, a[i]);
    }
    vi dp(maxx + 2);
    const ll MOD = 100000007LL;
    dp[1] = 1; dp[2] = 1;
    for(int i = 3;i <= maxx; i++) {
        dp[i] = (dp[i-1] + dp[i-2]) % MOD;
    }
    for(int i = 0; i < n; i ++) {
        cout << dp[a[i]] << endl;
    }
}

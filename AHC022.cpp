#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <queue>
#include <cmath>
#include <fstream>
#define INF 1e9
#define vi vector<int>
#define vb vector<bool>
#define vll vector<long long>
#define v2i vector<vector<int>>
#define v2b vector<vector<bool>>
#define all(x) x.begin(),x.end()
using namespace std;
using ll = long long;

struct vec2{
    int y, x; 
    bool operator<(const vec2 right){
        return y == right.y ? x < right.x : y < right.y;
    }
};

struct Solver{
    

    Solver solver()
    {}

    void solve(){


        return ;
    }
};



int main(){
    int l, n, s; cin >> l >> n >> s;
    vector<vec2> exitvec(n);
    //全て異なり、辞書順に与えられる
    for(int i=0; i<n; i++) cin >> exitvec[i].y >> exitvec[i].x;


    return 0;
}
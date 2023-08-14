#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <queue>
#include <cmath>
#include <fstream>
#include <time.h>
#include <random>
#define INF 1e9
#define vi vector<int>
#define vb vector<bool>
#define vll vector<long long>
#define v2i vector<vector<int>>
#define v2b vector<vector<bool>>
#define all(x) x.begin(),x.end()
using namespace std;
using ll = long long;


int Random(double min, double max){
    return (int)(min + (max - min) * rand() / (double)RAND_MAX);
}

int main(){
    srand((unsigned int)time(NULL));
    for(int i=0; i<10; i++){
        int fvalue = Random(0, 10000);
        cout << fvalue << endl;
    }
    return 0;
}
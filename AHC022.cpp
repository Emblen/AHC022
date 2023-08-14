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


const int max_measure = 10000;
const string inputfile = "input.txt";
const string outputfile = "output.txt";

struct vec2{
    int y, x; 
    bool operator<(const vec2 right){
        return y == right.y ? x < right.x : y < right.y;
    }
};


struct Judge {
    void cout_temperature(const v2i& temperature){
        for(int i=0; i<(int)temperature.size(); i++){
            for(int j=0; j<(int) temperature[i].size(); j++){
                cout << temperature[i][j] << " ";
            }
            cout << endl;
        }
    }

    void answer(const vi& estimate){
        cout << "-1 -1 -1" << endl;
        for(int v:estimate) cout << v << endl;
    }
};

struct Solver{
    const int L, N, S;
    const vector<vec2> exit_cell;
    Judge judge;

    Solver(int l, int n, int s, vector<vec2> exit_cell)
    : L(l), N(n), S(s), exit_cell(exit_cell)
    {}

    void solve(){


    }

    void calculate_cost(){

    }
};

struct LocalJudge {
    vector<vec2> exit_cell;
    vi ans, f;
    LocalJudge(vector<vec2> exit_cell, vi ans, vi f)
    : exit_cell(exit_cell), ans(ans), f(f) { }

    void cout_temperature(const v2i& temperature){
        ofstream output;
        output.open(outputfile);
        for(int i=0; i<(int)temperature.size(); i++){
            for(int j=0; j<(int) temperature[i].size(); j++){
                output << temperature[i][j] << " ";
            }
            output << endl;
        }
        output << endl;
        output.close();
    }

    int measure(int i, int y, int x, int mc, int pval){
        ofstream output;
        output.open(outputfile, ios::app);
        output << i << " " << y << " " << x << endl;

        int v = max(0, min(1000, pval + f[mc]));
        output << "# pre return value = " <<  v << endl;
        return v;
    }

    void answer(const vi& estimate){
        ofstream output;
        output.open(outputfile, ios::app);
        output << "-1 -1 -1" << endl;
        for(int v:estimate) output << v << endl;
        output.close();
        
        cout << "completed successfully" << endl;
    }

};

struct LocalSolver{
    const int L, N, S;
    const vector<vec2> exit_cell;
    const vi ans, f;
    LocalJudge localjudge;

    LocalSolver(int l, int n, int s, vector<vec2> exit_cell, vi ans, vi f)
    : L(l), N(n), S(s), exit_cell(exit_cell), ans(ans), f(f), localjudge(exit_cell, ans, f)
    { }

    void solve(){
        const v2i temperature = create_temperature();
        localjudge.cout_temperature(temperature);

        const pair<vi, int> estimate_rt = predict(temperature);
        const vi estimate = estimate_rt.first;
        const int measure_count = estimate_rt.second;
        localjudge.answer(estimate);
    }

    v2i create_temperature(){
        v2i temperature(L, vi(L, 0));
        /////////////ヒートマップを決める

        for (int i = 0; i < N; i++) {   
            temperature[exit_cell[i].y][exit_cell[i].x] = i * 10;
        }

        ////////////
        return temperature;
    }

    pair<vi, int> predict(const v2i& temperature){
        vi estimate(N);
        int measure_count = 0;
        //答え丸写し
        // for(int i=0; i<N; i++){
        //     estimate[i] = ans[i];
        // }
        ////////予測の方法を決める
        for (int i_in = 0; i_in < N; i_in++) {
            // you can output comment
            ofstream output;
            output.open(outputfile, ios::app);
            output << "# measure i=" << i_in << " y=0 x=0" << endl;
            output.close();

            vec2 land_pos = exit_cell[ans[i_in]];
            vec2 plus = {0, 0};
            int pval = temperature[land_pos.y+plus.y][land_pos.x+plus.x];
            int measured_value = localjudge.measure(i_in, 0, 0, measure_count, pval);
            measure_count++; 

            int min_diff = 9999;
            for (int i_out = 0; i_out < N; i_out++) {
                const vec2& pos = exit_cell[i_out];
                int diff = abs(temperature[pos.y][pos.x] - measured_value);
                if (diff < min_diff) {
                    min_diff = diff;
                    estimate[i_in] = i_out;
                }
            }
        }


        ///////////////////
        return {estimate, measure_count};
    }
};



int main(){
//Submit
    // int l, n, s; cin >> l >> n >> s;
    // vector<vec2> exit_cell(n);
    // //全て異なり、辞書順に与えられる
    // for(int i=0; i<n; i++) cin >> exit_cell[i].y >> exit_cell[i].x;

    // Solver solver(l, n, s, exit_cell);
    // solver.solve();

//Local
    ifstream input(inputfile);
    int l, n, s;
    vector<vec2> exit_cell(n);
    vi ans(n), f(max_measure);

    input >> l >> n >> s;
    for(int i=0; i<n; i++) input >> exit_cell[i].y >> exit_cell[i].x;
    for(int i=0; i<n; i++) input >> ans[i];
    for(int i=0; i<max_measure; i++) input >> f[i];
    input.close();

    LocalSolver localsolver(l, n, s, exit_cell, ans, f);
    localsolver.solve();

    
    return 0;
}
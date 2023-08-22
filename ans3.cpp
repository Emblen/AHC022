// 1つの出口セルを基準にする方法
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
#include <map>
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
    bool operator<(const vec2 right) const {
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

    int measure(int i, int y, int x){
        cout << i << " " << y << " " << x << endl;
        int v; cin >> v;
        if (v == -1) exit(1);
        return v;
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
        vec2 nearest = nearest_center();
        const v2i temperature = create_temperature(nearest);
        judge.cout_temperature(temperature);

        vector<vec2> diff_vec = calc_exitcell_diff(nearest);
        const vi estimate = predict(temperature, diff_vec);
        judge.answer(estimate);
    }

    //出口セルマップの中心から最も近い出口セルの座標を求める
    vec2 nearest_center(){
        vec2 nearst = {-1, -1};
        int m_dist = INF;
        int yx_diff = INF;
        const vec2 center = {L/2, L/2};
        
        for(int i=0; i<N; i++){
            vec2 tmp = exit_cell[i];
            int yd = abs(center.y - tmp.y);
            int xd = abs(center.x - tmp.x);

            if(m_dist > yd+xd){
                m_dist = yd + xd;
                nearst = tmp;
                yx_diff = abs(yd - xd);
            }
            else if(m_dist == yd+xd && yx_diff > abs(yd - xd)){
                nearst = tmp;
                yx_diff = abs(yd - xd);
            }
        }   
        return nearst;
    }

    //ヒートマップを生成する
    v2i create_temperature(vec2 pivot){
        v2i temperature(L, vi(L, 0));
        int temp = (S<40) ? 400 : max(min(400,(int)((double)S*2.4)), 1000);
        if(S<=25) temp = 200;
        // int temp = 1000;

        temperature[pivot.y][pivot.x] = temp;
        return temperature;
    }

    //出口セルの場所を2次元配列で保存する
    v2b create_cell_map(const vector<vec2>& exit_cell){
        v2b exit_cell_map(L, vb(L, false));
        for(auto v:exit_cell){
            exit_cell_map[v.y][v.x] = true;
        }
        return exit_cell_map;
    }

    //基準出口セルと他の出口セルのy方向、x方向の距離を求める
    vector<vec2> calc_exitcell_diff(vec2 pivot){
        vector<vec2> diff_vec(N);
        for(int i=0; i<N; i++){
            diff_vec[i] = {pivot.y - exit_cell[i].y, pivot.x - exit_cell[i].x};
        }
        return diff_vec;
    }

///////////////////予測の方法を決める//////////////////////////////////////////////

    vi predict(v2i temperature, vector<vec2> diff_vec){
        vi estimate(N, 0);
        int threshold = (S<40) ? 200 : max(200, min((int)((double)S*2.4)*3, 1000));  //2で割るだけだとやばそう
        if(threshold==1000) threshold -= S/4;
        if(S<=25) threshold = 100;
        cout << "# threshold = " << threshold << endl;
        
        map<vec2, int> diff_exitcellnum;
        for(int i=0; i<N; i++) diff_exitcellnum.insert({diff_vec[i], i});

        vector<vec2> diff_vec2 = diff_vec;

        sort(diff_vec2.begin(), diff_vec2.end(), [](const vec2& l, const vec2& r){
            int sl = abs(l.y)+abs(l.x);
            int sr = abs(r.y)+abs(r.x);
            return sl<sr;
        });
        
        for(int i=0; i<N; i++){
            int mxt = -1;
            vec2 mxt_vec;
            bool isconf = false;

            for(int j=0; j<(int)diff_vec2.size(); j++){
                int t = judge.measure(i, diff_vec2[j].y, diff_vec2[j].x);

                if(t>threshold){
                    estimate[i] = diff_exitcellnum[diff_vec2[j]];
                    diff_vec2.erase(diff_vec2.begin()+j);
                    isconf = true;
                    break;
                }
                if(mxt<t){
                    mxt = t;
                    mxt_vec = diff_vec2[j];
                }
            }
            if(!isconf) estimate[i] = diff_exitcellnum[mxt_vec];
        } 
        
        return estimate;
    }
};


int main(){
//Submit
    int l, n, s; cin >> l >> n >> s;
    vector<vec2> exit_cell(n);
    //全て異なり、辞書順に与えられる
    for(int i=0; i<n; i++) cin >> exit_cell[i].y >> exit_cell[i].x;

    Solver solver(l, n, s, exit_cell);
    solver.solve();

    cout << "# completed successfully" << endl;
    return 0;
}
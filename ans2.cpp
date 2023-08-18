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

const int init_tmp = 1000;
const int init_threshold = 600;


const int max_measure = 10000;
const string inputfile = "input.txt";
const string outputfile = "output.txt";

const vi temp_vec = {400, 400, 500, 800, 800, 1000, 1000, 1000, 1000, 1000};
const vi threshold_vec = {200, 200, 350, 600, 600, 800, 800, 800, 800, 800};

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
        cout << "# threshold = " << threshold << endl;
        
        map<vec2, int> diff_exitcellnum;
        for(int i=0; i<N; i++) diff_exitcellnum.insert({diff_vec[i], i});

        vector<vec2> diff_vec2 = diff_vec;
        
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


struct LocalJudge {
    vector<vec2> exit_cell;
    vi measure_cost;
    vi ans, f;
    LocalJudge(vector<vec2> exit_cell, vi ans, vi f)
    : exit_cell(exit_cell), ans(ans), f(f) { }

    //ヒートマップを出力する
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

    //答えを予測するための計測を行う
    int measure(int i, int y, int x, int pval){
       
        //インタラクティブ形式の戻り値
        //pvalは、ワームホールiに対応する出口セルの座標+移動量の座標の温度
        //f[(int)measure_cost.size()]は、正規分布からとってきた擬似値
        int v = max(0, min(1000, pval + f[(int)measure_cost.size()]));


        ofstream output;
        output.open(outputfile, ios::app);
        output << "# return value = " << v << endl;
        output << i << " " << y << " " << x << endl;
        output.close();

        measure_cost.push_back(10 + abs(y) + abs(x));
        return v;
    }

    void answer(const vi& estimate){
        ofstream output;
        output.open(outputfile, ios::app);
        output << "-1 -1 -1" << endl;
        for(int v:estimate) output << v << endl;
        output.close();
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
        //中心に一番近い出口セルを求め、基準出口セルにする
        vec2 nearest = nearest_center();
        //ヒートマップを生成
        const v2i temperature = create_temperature(nearest);
        //ヒートマップを出力
        localjudge.cout_temperature(temperature);
        //基準出口セルと他の全ての出口セルの座標の差を計算する
        vector<vec2> diff_vec = calc_exitcell_diff(nearest);
        //ワームホールと出口セルの対応関係を予測する
        const vi estimate = predict(temperature, diff_vec);
        //予測したものを出力する
        localjudge.answer(estimate);
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
        cout << "# temperature = " << temp << endl;
        // int temp = init_tmp;

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
        int threshold = (S<40) ? 200 : max(200, min((int)((double)S*1.2), 1000));  //2で割るだけだとやばそう
        if(threshold==1000) threshold -= S/4;
        // int threshold = init_threshold;
        // int threshold = 900;
        
        cout << "# threshold = " << threshold << endl;
        // 0. diff_vecと出口セルの番号を対応させるmapを作成
        // 1. diff_vecをシャッフル
        //     2. ワームホールi,diff_y,diff_xを入力として与える
        //     3. 返ってきた値から、基準セルかどうか判断する
        //     4. 基準セルと判断した場合、作成したmapにdiffを入れて出口セルの番号を取り出し、ワームホールiの出口セルの予測値とする。
        //     基準セルではないと判断した場合、返ってきた値の最大値と出口セルの番号を記録して2以降を行う。
        //     5. 予測値を決めた場合、このdiffを配列から削除する。
        // 全てのiについてこれを行う   
        map<vec2, int> diff_exitcellnum;
        for(int i=0; i<N; i++) diff_exitcellnum.insert({diff_vec[i], i});

        vector<vec2> diff_vec2 = diff_vec;
        
        for(int i=0; i<N; i++){
            int mxt = -1;
            vec2 mxt_vec;
            bool isconf = false;

            for(int j=0; j<(int)diff_vec2.size(); j++){
                //localでは、参照するセルの温度を渡す
                int py = exit_cell[ans[i]].y + diff_vec2[j].y;
                int px = exit_cell[ans[i]].x + diff_vec2[j].x;
                py = (py+L)%L;
                px = (px+L)%L;
                int pval = temperature[py][px];

                int t = localjudge.measure(i, diff_vec2[j].y, diff_vec2[j].x, pval);

                if(t>threshold){
                    estimate[i] = diff_exitcellnum[diff_vec2[j]];
                    diff_vec2.erase(diff_vec2.begin()+j);
                    isconf = true;
                    // cout << estimate[i] << " ";
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

//Local
    // ifstream input(inputfile);
    // int l, n, s;
    // vector<vec2> exit_cell(n);
    // vi ans(n), f(max_measure);

    // input >> l >> n >> s;
    // for(int i=0; i<n; i++) input >> exit_cell[i].y >> exit_cell[i].x;
    // for(int i=0; i<n; i++) input >> ans[i];
    // for(int i=0; i<max_measure; i++) input >> f[i];
    // input.close();

    // LocalSolver localsolver(l, n, s, exit_cell, ans, f);
    // localsolver.solve();

    cout << "# completed successfully" << endl;
    return 0;
}
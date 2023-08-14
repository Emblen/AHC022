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
        const v2i temperature = create_temperature();
        judge.cout_temperature(temperature);//ヒートマップ出力

        const v2b exit_cell_map = create_cell_map(exit_cell);//出口セルか否かのマップ
        const vector<vector<vec2>> neighbor = BFS(exit_cell_map);

        const vi estimate = predict(temperature, neighbor);
        judge.answer(estimate);

    }

    v2b create_cell_map(const vector<vec2>& exit_cell){
        v2b exit_cell_map(L, vb(L, false));
        for(auto v:exit_cell){
            exit_cell_map[v.y][v.x] = true;
        }
        return exit_cell_map;
    }

    //ある出口セルの近傍の出口セルをBFSでnb個求める
    vector<vector<vec2>> BFS(v2b exit_cell_map){
        vector<vector<vec2>> neighbor(N);
        const vector<vec2> dydx = {{1,0},{0,1},{-1,0},{0,-1}};
        const int nb = 2;//近傍の数

        for(int i=0; i<N; i++){
            vec2 cell = exit_cell[i];
            v2b visit(L, vb(L, false));
            
            //マンハッタン距離が小さい順にキューから取り出す
            auto c = [](pair<int, vec2> l, pair<int, vec2> r){
                return l.first==r.first ? l.second<r.second : l.first>r.first;
            };
            priority_queue<pair<int, vec2>, vector<pair<int, vec2>>, decltype(c)> pq(c);
            pq.push({0, cell});

            int cnt = -1;
            while(cnt<nb){
                pair<int, vec2> tmp = pq.top();
                pq.pop();
                int m_dist = tmp.first;
                vec2 pv = tmp.second;

                if(visit[pv.y][pv.x]) continue;
                if(exit_cell_map[pv.y][pv.x]){
                    if(cnt>=0) neighbor[i].push_back(pv);
                    cnt++;
                }    
                visit[pv.y][pv.x] = true;

                for(auto d:dydx){
                    int cy = pv.y + d.y;
                    int cx = pv.x + d.x;
                    if(cy<0 || cy>=L || cx<0 || cx>=L || visit[cy][cx]) continue;
                    pq.push({m_dist+1, {cy, cx}});
                }
            }
        }

        for(int i=0; i<N; i++){
            cout << "# (" << exit_cell[i].y << "," << exit_cell[i].x << ")" << ": ";
            for(int j=0; j<nb; j++){
                cout << "(" << neighbor[i][j].y << "," << neighbor[i][j].x << ")" << " ";
            }
            cout << endl;
        }
        return neighbor;
    }

    v2i create_temperature(){
        v2i temperature(L, vi(L, 0));
        for (int i = 0; i < N; i++) {
            temperature[exit_cell[i].y][exit_cell[i].x] = 800;
        }
        return temperature;
    }
    
    vi predict(const v2i& temperature, vector<vector<vec2>> neighbor){
        set<vec2> exit_set;
        for(int i=0; i<N; i++) exit_set.insert(exit_cell[i]);
        map<vec2, int> exit_map;
        for(int i=0; i<N; i++) exit_map.insert({exit_cell[i], i});

        vi estimate(N, 0);
        const int threshold = 600;

        for(int i=0; i<N; i++){
            int mxcnt = -1, mxj = -1;
            bool assigned = false;

            for(auto cell:exit_set){
                vec2 base = cell;

                bool flag = true;
                int truecnt = 0;
                int j = exit_map[cell];
                for(int k=0; k<(int)neighbor[j].size(); k++){
                    vec2 nb = neighbor[j][k];
                    int dy = nb.y - base.y;
                    int dx = nb.x - base.x;
                    
                    int v = judge.measure(i, dy, dx);

                    //出口セルか否かここで判断する
                    if(v < threshold) flag = false;
                    else truecnt++;
                }
                //フラグがtrueなら全てが出口セルだったので元の出口セルを予測値とする
                if(flag){
                    assigned = true;
                    estimate[i] = j; 
                    exit_set.erase(cell);
                    break;
                }
                //フラグがfalseなら、一部または全部出口セルでなかったので出口セルだった数とjの値を記録しておく
                if(mxcnt<truecnt){
                    mxcnt = truecnt; 
                    mxj = j;
                }
            }
            if(!assigned) estimate[i] = mxj; //一番多く出口セルだったjを予測値として採用する
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
        // 1. ヒートマップ作成
        // 2. ヒートマップ出力
        // 3. ヒートマップを用いてワームホールと出口セルの対応を予測
        // 4. 予測した 0 ~ N-1 の順列（重複可能）を予測して出力する
        const v2i temperature = create_temperature();
        localjudge.cout_temperature(temperature);

        const v2b exit_cell_map = create_cell_map(exit_cell);//出口セルか否かのマップ
        const vector<vector<vec2>> neighbor = BFS(exit_cell_map);

        const vi estimate = predict(temperature, neighbor);
        localjudge.answer(estimate);
    }

    v2i create_temperature(){
        v2i temperature(L, vi(L, 0));
        /////////////ヒートマップを決める

        for (int i = 0; i < N; i++) {   
            temperature[exit_cell[i].y][exit_cell[i].x] = 800;
        }

        ////////////
        return temperature;
    }

    v2b create_cell_map(const vector<vec2>& exit_cell){
        v2b exit_cell_map(L, vb(L, false));
        for(auto v:exit_cell){
            exit_cell_map[v.y][v.x] = true;
        }
        return exit_cell_map;
    }

    //ある出口セルの近傍の出口セルをBFSでnb個求める
    vector<vector<vec2>> BFS(v2b exit_cell_map){
        vector<vector<vec2>> neighbor(N);
        const vector<vec2> dydx = {{1,0},{0,1},{-1,0},{0,-1}};
        const int nb = 2;//近傍の数

        for(int i=0; i<N; i++){
            vec2 cell = exit_cell[i];
            v2b visit(L, vb(L, false));
            
            //マンハッタン距離が小さい順にキューから取り出す
            auto c = [](pair<int, vec2> l, pair<int, vec2> r){
                return l.first==r.first ? l.second<r.second : l.first>r.first;
            };
            priority_queue<pair<int, vec2>, vector<pair<int, vec2>>, decltype(c)> pq(c);
            pq.push({0, cell});

            int cnt = -1;
            while(cnt<nb){
                pair<int, vec2> tmp = pq.top();
                pq.pop();
                int m_dist = tmp.first;
                vec2 pv = tmp.second;

                if(visit[pv.y][pv.x]) continue;
                if(exit_cell_map[pv.y][pv.x]){
                    if(cnt>=0) neighbor[i].push_back(pv);
                    cnt++;
                }    
                visit[pv.y][pv.x] = true;

                for(auto d:dydx){
                    int cy = pv.y + d.y;
                    int cx = pv.x + d.x;
                    if(cy<0 || cy>=L || cx<0 || cx>=L || visit[cy][cx]) continue;
                    pq.push({m_dist+1, {cy, cx}});
                }
            }
        }

        for(int i=0; i<N; i++){
            cout << "# (" << exit_cell[i].y << "," << exit_cell[i].x << ")" << ": ";
            for(int j=0; j<nb; j++){
                cout << "(" << neighbor[i][j].y << "," << neighbor[i][j].x << ")" << " ";
            }
            cout << endl;
        }
        return neighbor;
    }


///////////////////予測の方法を決める//////////////////////////////////////////////

    vi predict(const v2i& temperature, vector<vector<vec2>> neighbor){

        set<vec2> exit_set;
        for(int i=0; i<N; i++) exit_set.insert(exit_cell[i]);
        //該当出口セルが見つかったらeraseする。これで計算量減らせるね。
        map<vec2, int> exit_map;
        for(int i=0; i<N; i++) exit_map.insert({exit_cell[i], i});

        vi estimate(N, 0);
        const int threshold = 600;//σと初期ヒートマップの値によって可変にしたら良さそう

        //全てのワームホールについて、ある出口セルとその近傍の2つの出口セルの差分を出力し、温度の計測を行うことで出口セルかどうか判別する。
        for(int i=0; i<N; i++){
            int mxcnt = -1, mxj = -1;
            bool assigned = false;
            vec2 ansi_pos = exit_cell[ans[i]];

            // cout << (int)exit_set.size() << " ";
            for(auto cell:exit_set){ //近傍の出口セルを計測先として指定し、そこが出口セルかどうか見極める。これによって元の出口セルが判別できる。
                vec2 base = cell;

                bool flag = true;
                int truecnt = 0;
                int j = exit_map[cell];
                for(int k=0; k<(int)neighbor[j].size(); k++){
                    vec2 nb = neighbor[j][k];
                    int dy = nb.y - base.y;
                    int dx = nb.x - base.x;

                    ofstream output;
                    output.open(outputfile, ios::app);
                    output << "# measure i=" << i << " y=" << dy << " x=" << dx << endl;
                    
                    
                    
                    vec2 mp = {ansi_pos.y+dy, ansi_pos.x+dx};
                    if(mp.y<0 || mp.y>=L || mp.x<0 || mp.x>=L) {flag = false; continue;}
                    output << "# mp_pos = ("<<mp.y << ","<<mp.x<<")" << endl;
                    output.close();

                    int pval = temperature[mp.y][mp.x];
                    int v = localjudge.measure(i, dy, dx, pval);

                    //出口セルか否かここで判断する
                    if(v < threshold) flag = false;
                    else truecnt++;
                }
                //フラグがtrueなら全てが出口セルだったので元の出口セルを予測値とする
                if(flag){
                    assigned = true;
                    estimate[i] = j; 
                    exit_set.erase(cell);
                    break;
                }
                //フラグがfalseなら、一部または全部出口セルでなかったので出口セルだった数とjの値を記録しておく
                if(mxcnt<truecnt){
                    mxcnt = truecnt; 
                    mxj = j;
                }
            }
            if(!assigned) estimate[i] = mxj; //一番多く出口セルだったjを予測値として採用する
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

    // cout << "# completed successfully" << endl;
    return 0;
}
#include <vector>
#include <algorithm> // std::max, std::find, std::reverse, std::count
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <utility>   // std::pair
#include "graph.h"

using std::vector;
using std::max;
using std::find;
using std::unordered_map;
using std::unordered_set;
using std::pair;
using std::reverse;
using std::cout;
using std::endl;
using std::queue;
using std::count;



// debug
void print_vector(const vector<int>& vec) {
    for (int i =0 ; i < vec.size(); ++i) {
        if (i == 0) {
            cout << "[ ";
        }
        cout << vec[i] << " ";
        if (i == vec.size() - 1) {
            cout << "]" << endl;
        }
    }
}



// 重要な関数が上に来るように下位の関数は前方宣言
vector<int> get_path_from_sketch(
    const vector<vector<int> >& sketch,
    int node);

vector<int> concatenate_path(
    vector<int> path_to_be_added,
    const vector<int>& path_to_add);

bool has_cycle_for_path(const vector<int>& path);


/* 事前計算 */
// takishi 追加
// sketch_node と Si の最短経路を返す
// 引数 : sketch_node, 集合 Si
// 返り値 : sketch_node と Si の最短経路
std::vector<int> bfs_to_seed_node(
    const Graph& graph,
    int sketch_node,
    const unordered_set<int>& seed_node_set)
{
    vector<int> shortest_path;
    if (seed_node_set.count(sketch_node)) { // sketch_node が Si に含まれているとき
        shortest_path.push_back(sketch_node);
        return shortest_path;
    }

    int n = graph.get_number_of_nodes();
    std::vector<int> dist(n, 1e9);
    std::vector<int> pre(n, -1); // 経路として見たとき一個前の頂点
    std::queue<int> que;
    que.push(sketch_node);
    dist[sketch_node] = 0;

    int target = 0; // Si に属する頂点のうち, sketch_node から一番近いもの

    while (!que.empty()) {
        // 現在頂点
        int from = que.front();
        que.pop();

        // Si に属する頂点に辿り着いたかどうか
        bool find_Si = false;

        for (const int& to : graph.get_adjacency_list().at(from)) { // from の隣接頂点を見ていく
            if (dist[to] != 1e9) continue;
            dist[to] = dist[from] + 1;
            pre[to] = from;

            if (seed_node_set.count(to)) { // to が Si に属するならそこで終了
                target = to;
                find_Si = true;
                break;
            } 

            que.push(to);
        }

        if (find_Si) break;
    }

    // target から巻き戻し, 最短経路を構築
    for (int i = target; i != -1; i = pre[i]) {
        shortest_path.push_back(i);
    }
    reverse( shortest_path.begin(), shortest_path.end() );

    return shortest_path;
}


// sketch 生成
vector<vector<int> > sketch_index(
    const Graph& graph,
    int sketch_node,
    const vector<unordered_set<int> >& seed_node_sets)
{
    vector<vector<int> > sketch; // sketch_node の sketch : [S1との最短経路, S2との最短経路, ..., Smとの最短経路]
    vector<int> shortest_path;

    /* sketch_node と Si の最短経路を探す */
    for (int i = 0; i < seed_node_sets.size(); ++i) {
        sketch.push_back(bfs_to_seed_node(graph, sketch_node, seed_node_sets[i]));
    }


    return sketch;
}


/* 実行時計算 */
// sketch を bfs
vector<int> bfs_sketch(
    int sketch_node,
    const vector<vector<int> >& sketch)
{
    vector<int> visited_nodes{sketch_node};

    // bfs する最大の深さ (最も長い最短経路の長さ) を調べる
    int max_shortest_path_size = 0;
    for (const vector<int>& shortest_path : sketch) {
        int tmp_size = static_cast<int>( shortest_path.size() );
        if (max_shortest_path_size < tmp_size) {
            max_shortest_path_size = tmp_size;
        }
    }
    
    // 外側の for は深さの制御, 内側の for は幅の制御
    for (int i = 1; i < max_shortest_path_size; ++i) { // sketch 中の最短経路の 1 つ目のノードは sketch_node 自身であるため 1 からスタート
        for (const vector<int>& shortest_path : sketch  ) {
            if ( static_cast<int>( shortest_path.size() ) < (i + 1) ) {
                continue; // 探索中の深さより短い最短経路は無視
            }
            if (find(
                visited_nodes.begin(),
                visited_nodes.end(),
                shortest_path[i]) == visited_nodes.end() ) {
                    visited_nodes.push_back(shortest_path[i]); // visited_nodes になければ追加
                }
        }
    }

    return visited_nodes;
}


// sketchLS
Graph sketchLS(
    const Graph& graph,
    vector<int> terminals,
    const unordered_map<int, vector<vector<int> > >& sketches)
{
    /* ターミナルの sketch を読み込み */
    unordered_map<int, vector<vector<int> > > sketch_of_terminals;
    for (int terminal : terminals) {
        sketch_of_terminals[terminal] = sketches.at(terminal);
    }

    /* ターミナルごとに BFS */
    unordered_map<int, vector<int> > BFS_of_terminals;
    int max_BFS_size = 0;
    for (int terminal : terminals) {
        vector<int> tmp_BFS = bfs_sketch(terminal, sketch_of_terminals[terminal]);
        BFS_of_terminals[terminal] = tmp_BFS;
        int tmp_BFS_size = static_cast<int>( tmp_BFS.size() );
        if (tmp_BFS_size > max_BFS_size) {
            max_BFS_size = tmp_BFS_size;
        }
    }

    // debug
    // for (const pair<int, vector<int> >& item : BFS_of_terminals) {
    //     cout << item.first << " : ";
    //     print_vector(item.second);
    // }

    unordered_map<int, vector<int> > visited_nodes_of_terminals; // ターミナル毎の訪問したノード
    vector<int> set_of_covered_terminals;                        // カバーしたターミナル

    Graph T; // シュタイナー木
    
    /* round-robin 方式で BFS をローカルサーチ (探索中のノードが訪問済みノードと隣接かどうか確認) */
    for (int i = 0; i < max_BFS_size; ++i) {
        for (const pair<int, vector<int> >& item : BFS_of_terminals ) {
            int searching_terminal = item.first;       // 探索中のターミナル
            vector<int> searching_BFS = item.second;   // 探索中の BFS

            if (static_cast<int>( searching_BFS.size() ) < i + 1) {
                continue;
            }
            int v = searching_BFS[i];                                       // 探索中のノード
            visited_nodes_of_terminals[searching_terminal].push_back(v);    // v を訪問済みに
            vector<int> neighbors_of_v = graph.get_adjacency_list().at(v);  // v の隣接ノードのリスト



            // v の隣接ノードが探索中でないターミナルの訪問ノードにあればその path を追加 (閉路ができない場合)
            for (int other_terminal : terminals) {
                if (other_terminal == searching_terminal) {
                    continue; // 探索中のターミナルは飛ばす
                }
                if ( visited_nodes_of_terminals[other_terminal].empty() ) {
                    continue; // 訪問したノードが 1 つもなければ飛ばす
                }
                for (int neighbor_of_v : neighbors_of_v) {
                    vector<int>::iterator find_itr = find(
                        visited_nodes_of_terminals[other_terminal].begin(),
                        visited_nodes_of_terminals[other_terminal].end(),
                        neighbor_of_v
                    );
                    
                    if ( find_itr != visited_nodes_of_terminals[other_terminal].end() ) {
                        /* [searching_terminal, ..., v, n, ..., other_terminal] を生成 */
                        int n = *find_itr; // v に隣接し, terminal の BFS で訪問済みのノード

                        vector<int> path_from_searching_terminal 
                            = get_path_from_sketch(sketch_of_terminals[searching_terminal], v);
                        vector<int> path_to_other_terminal
                            = get_path_from_sketch(sketch_of_terminals[other_terminal], n);     // この時点ではまだ [other_terminal, ..., n] となっている
                        reverse(path_to_other_terminal.begin(), path_to_other_terminal.end());  // 逆順にソート [n, ..., other_terminal] とする
                        
                        vector<int> tmp_path = concatenate_path(path_from_searching_terminal, path_to_other_terminal);

                        // debug
                        // cout << "tmp_path : ";
                        // print_vector(tmp_path);

                        /* tmp_path 自身が閉路を持つか確認 */
                        if ( has_cycle_for_path(tmp_path) ) {

                            // // debug
                            // cout << "tmp_path has cycle" << endl;

                            continue;
                        }

                        /* tmp_path の追加で閉路ができないか確認 */
                        Graph Tcopy{T};
                        Tcopy.add_path(tmp_path);
                        if ( Tcopy.has_cycle() ) {

                            // // debug
                            // cout << "tmp_path make cycle" << endl;

                            continue;
                        }

                        // debug
                        // cout << "tmp_path doesnt make cycle" << endl;

                        /* path の追加と set_of_covered_terminals への追加 */
                        T.add_path(tmp_path);
                        // set_of_covered_terminals になければ追加
                        if ( find(set_of_covered_terminals.begin(), set_of_covered_terminals.end(), searching_terminal) == set_of_covered_terminals.end() ) {
                            set_of_covered_terminals.push_back(searching_terminal);
                        }
                        if ( find(set_of_covered_terminals.begin(), set_of_covered_terminals.end(), other_terminal) == set_of_covered_terminals.end() ) {
                            set_of_covered_terminals.push_back(other_terminal);
                        }

                        if (T.is_connected() && ( set_of_covered_terminals.size() == terminals.size() ) ) {
                            return T;
                        }

                        // debug
                        // cout << "set_of_covered_terminals : ";
                        // print_vector(set_of_covered_terminals);
                        // cout << "Is T connected ? " << (T.is_connected() ? "True" : "False") << endl;
                    }
                }  
            }
        }
    }
}


// partial_sketches に対する sketchLS
Graph partial_sketchLS(
    const Graph& graph,
    vector<int> terminals,
    const unordered_map<int, vector<vector<int> > >& partial_sketches)
{
    // sketch を持つノードの集合を作成
    unordered_set<int> nodes_having_sketch;
    for (const pair<int, vector<vector<int> > >& item : partial_sketches) {
        nodes_having_sketch.insert(item.first);
    }

    // sketch を持たないターミナルの置き換え
    vector<pair<pair<int, int>, vector<int> > > pair_of_terminals_and_shortest_path; // <<置き換え前ノード, 置き換え後ノード>, 2 つの最短経路>
    vector<int> alternative_terminals;
    unordered_map<int, vector<vector<int> > > alternative_sketches;
    for (int terminal : terminals) {
        if (nodes_having_sketch.count(terminal) != 0) { // sketch を持つならそのまま
            if (find(alternative_terminals.begin(), alternative_terminals.end(), terminal) == alternative_terminals.end()) {
                alternative_terminals.push_back(terminal);
                alternative_sketches[terminal] = partial_sketches.at(terminal);
            }
        } else { 
            // 近傍のsketchを持つノードを探索し置き換え
            vector<int> shortest_path = bfs_to_seed_node(graph, terminal, nodes_having_sketch); // ToDo : 関数名を変える
            int alternative_node = shortest_path.back();
            pair_of_terminals_and_shortest_path.push_back({ {terminal, alternative_node}, shortest_path } );
            if (find(alternative_terminals.begin(), alternative_terminals.end(), alternative_node) == alternative_terminals.end()) {
                alternative_terminals.push_back(alternative_node);
                alternative_sketches[alternative_node] = partial_sketches.at(alternative_node);
            }
        }
    }

    // 置き換え後のターミナルに対して sketchLS
    Graph SteinerTree = sketchLS(graph, alternative_terminals, alternative_sketches);

    // 置き換え前後の最短経路を追加
    for (const pair<pair<int, int>, vector<int> >& item : pair_of_terminals_and_shortest_path) {
        SteinerTree.add_path(item.second);
    }

    return SteinerTree;
}


/* 下位関数 */
// sketch から node までの最短経路を取得
vector<int> get_path_from_sketch(
    const vector<vector<int> >& sketch,
    int node)
{
    vector<int> shortest_path_to_node;
    for (vector<int> shortest_path : sketch) {
        vector<int>::iterator find_itr = find(
            shortest_path.begin(),
            shortest_path.end(),
            node
        );
        if ( find_itr != shortest_path.end() ) {
            vector<int> tmp_path(shortest_path.begin(), find_itr + 1);
            if ( shortest_path_to_node.empty() ) {
                shortest_path_to_node = tmp_path;
                continue;
            }
            if ( tmp_path.size() < shortest_path_to_node.size() ) {
                shortest_path_to_node = tmp_path;
            }
        }
    }

    return shortest_path_to_node;
}


// path 同士を結合
vector<int> concatenate_path(
    vector<int> path_to_be_added,
    const vector<int>& path_to_add)
{
    path_to_be_added.insert(
        path_to_be_added.end(),
        path_to_add.begin(),
        path_to_add.end()
    );

    return path_to_be_added;
}


// path 自身が閉路を持つか調べる. 持っていれば true, そうでなければ false
bool has_cycle_for_path(const vector<int>& path) {
    for (const int& node : path) {
        if (count(path.begin(), path.end(), node) > 1) {
            return true;
        }
    }

    return false;
}
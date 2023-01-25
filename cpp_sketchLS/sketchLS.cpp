#include <vector>
#include <algorithm> // std::max, std::find, std::reverse, std::count
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <queue>
#include <chrono>
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
using std::string;

namespace ch = std::chrono;


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





/* 重要な関数が上に来るように下位の関数は前方宣言 */
vector<int> get_path_from_sketch(
    const vector<vector<int> >& sketch,
    int node);

vector<int> concatenate_path(
    vector<int> path_to_be_added,
    const vector<int>& path_to_add);

bool has_cycle_for_path(const vector<int>& path);

void remove_unnecessary_path_from_ST(Graph& ST, vector<int> terminals);

void add_edges_til_terminal_connects_to_ST(Graph& ST, const vector<int>& path);

vector<int> get_shortest_path_from_SPT(
    const vector<int>& shortest_path_tree,
    const int& source);

vector<int> get_avoided_path_from_APT(
    const vector<int>& avoided_path_tree,
    const int& source);


/* 事前計算 */

// 各Si毎に最短路木を生成
// 最短路木を基にグラフ上の各ノードに対して, Sketch を生成
void generate_sketches(
    const Graph& graph,
    const vector<unordered_set<int> >& seed_node_sets,
    vector<vector<vector<int> > >& sketches)
{
    using Sketch = vector<vector<int> >;

    // 最短路木のリストを用意 [S1の最短路木, S2の最短路木, ..., Smの最短路木]
    vector<vector<int> > shortest_path_tree_list(seed_node_sets.size());

    #pragma omp parallel for
    for (int i = 0; i < seed_node_sets.size(); ++i) {
        vector<int> shortest_path_tree = graph.bfs_from_source_node_set(seed_node_sets.at(i));

        shortest_path_tree_list[i] = shortest_path_tree;
    }


    // 各ノードに対して Sketch を生成
    // 具体的には, 各シードノード集合の最短路木をたどってシードノード集合までの最短路を取得
    vector<int> node_list = graph.get_node_list();

    // #pragma omp parallel for
    for (int i = 0; i < node_list.size(); ++i) {
        Sketch sketch;
        
        for (const vector<int>& shortest_path_tree : shortest_path_tree_list) {
            vector<int> shortest_path_to_seed_node_set
                = get_shortest_path_from_SPT(shortest_path_tree, node_list.at(i));
            
            if (shortest_path_to_seed_node_set.empty()) {
                cout << "empty" << endl;
                throw;
            }

            sketch.push_back(shortest_path_to_seed_node_set);
        }

        sketches[node_list.at(i)] = sketch;
    }
}




// 各Si毎に 最短路木と avoided_path_tree (上位ノードの集合の数だけ) を生成
// それらを基に extended_sketch を生成
void generate_extended_sketches(
    const Graph& graph,
    const vector<unordered_set<int> >& seed_node_sets,
    const vector<unordered_set<int> >& top_node_sets,
    vector<vector<vector<vector<int> > > >& extended_sketches,
    vector<double>& precomputation_time_ms_list)
{
    using Path_List = vector<vector<int> >;     // Si の経路リスト = [最短経路, 上位1個避けた経路, ...]
    using Extended_Sketch = vector<Path_List>;  // Extended_Sketch = [S1の経路リスト, S2の経路リスト, ...]

    // path_tree のリストをシードノード集合毎に用意
    // 外側のvector S1, S2, ...
    // 内側のvector 最短路, 上位○個避けた経路, ...
    vector<vector<vector<int> > > list_of_list_of_path_tree(seed_node_sets.size());

    


    // 時間計測の準備
    // 並列化でも時間が測れるよう2次元vectorを用意
    // 外側 xの値 [original, 上位○個, 上位○個]
    // 内側 シードノード集合 [S1, S2, ..., Sm]
    vector<vector<double> > list_of_list_of_precomputation_time_ms1(
        precomputation_time_ms_list.size(),
        vector<double>(seed_node_sets.size(), 0)
    );




    #pragma omp parallel for
    for (int i = 0; i < seed_node_sets.size(); ++i) {
        // 時間計測の準備
        ch::system_clock::time_point start, end;
        

        vector<vector<int> > path_tree_list;

        // 最短路木を追加
        start = ch::system_clock::now();
        path_tree_list.push_back(graph.bfs_from_source_node_set(seed_node_sets.at(i)));
        end   = ch::system_clock::now();

        list_of_list_of_precomputation_time_ms1[0][i] += static_cast<double>(ch::duration_cast<ch::milliseconds>(end - start).count());


        // avoided_path_tree を追加
        for (int j = 0; j < top_node_sets.size(); ++j) {
            start = ch::system_clock::now();
            path_tree_list.push_back(
                graph.bfs_from_source_node_set_avoiding_another_node_set(
                    seed_node_sets.at(i),
                    top_node_sets.at(j))
            );
            end   = ch::system_clock::now();

            list_of_list_of_precomputation_time_ms1[j + 1][i] += static_cast<double>(ch::duration_cast<ch::milliseconds>(end - start).count());
        }

        list_of_list_of_path_tree[i] = path_tree_list;
    }




    // 時間計測のための処理
    // シードノード集合毎に計測した時間を合計する
    for (int i = 0; i < precomputation_time_ms_list.size(); ++i) {
        double total = 0;

        for (int j = 0; j < list_of_list_of_precomputation_time_ms1.at(i).size(); ++j) {
            total += list_of_list_of_precomputation_time_ms1.at(i).at(j);
        }

        precomputation_time_ms_list[i] += total;
    }




    vector<int> node_list = graph.get_node_list();




    // 時間計測のための準備
    // 並列化でも時間が測れるよう2次元vectorを用意
    // 外側 xの値 [original, 上位○個, 上位○個]
    // 内側 ノードリスト
    vector<vector<double> > list_of_list_of_precomputation_time_ms2(
        precomputation_time_ms_list.size(),
        vector<double>(node_list.size(), 0)
    );




    // 各ノードに対して extended_sketch を生成
    #pragma omp parallel for
    for (int i = 0; i < node_list.size(); ++i) {
        // 時間計測のための準備
        ch::system_clock::time_point start, end;


        Extended_Sketch extended_sketch;

        for (const vector<vector<int> >& path_tree_list : list_of_list_of_path_tree) {
            Path_List path_list;

            // 最短路を追加
            start = ch::system_clock::now();
            path_list.push_back(get_shortest_path_from_SPT(path_tree_list.at(0), node_list.at(i)));
            end   = ch::system_clock::now();

            list_of_list_of_precomputation_time_ms2[0][i] += static_cast<double>(ch::duration_cast<ch::milliseconds>(end - start).count());

            // avoided_path を追加
            for (int j = 1; j < path_tree_list.size(); ++j) {
                start = ch::system_clock::now();
                vector<int> avoided_path = get_avoided_path_from_APT(path_tree_list.at(j), node_list.at(i));

                // avoided_path が空vector, 即ち避けた経路が見つからなかった場合
                if ( avoided_path.empty() ) {
                    end = ch::system_clock::now();
                    list_of_list_of_precomputation_time_ms2[j][i] += static_cast<double>(ch::duration_cast<ch::milliseconds>(end - start).count());
                    break;
                }

                // 見つかれば追加
                path_list.push_back(avoided_path);
                end = ch::system_clock::now();
                list_of_list_of_precomputation_time_ms2[j][i] += static_cast<double>(ch::duration_cast<ch::milliseconds>(end - start).count());
            }

            extended_sketch.push_back(path_list);
        }

        extended_sketches[node_list.at(i)] = extended_sketch;
    }




    // 時間計測のための処理
    // ノード毎に計測した時間を合計する
    for (int i = 0; i < precomputation_time_ms_list.size(); ++i) {
        double total = 0;

        for (int j = 0; j < list_of_list_of_precomputation_time_ms2.at(i).size(); ++j) {
            total += list_of_list_of_precomputation_time_ms2.at(i).at(j);
        }

        precomputation_time_ms_list[i] += total;
    }
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

                        /* tmp_path 自身が閉路を持つか確認 */
                        if ( has_cycle_for_path(tmp_path) ) {
                            continue;
                        }

                        /* tmp_path の追加で閉路ができないか確認 */
                        Graph Tcopy{T};
                        Tcopy.add_path(tmp_path);
                        if ( Tcopy.has_cycle() ) {
                            continue;
                        }

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
                    }
                }  
            }
        }
    }
}




/* extended sketches を扱う関数 */
// 避けて経路を生成できた上位ノードの個数の最大値を返す
int get_max_number_of_avoided_top_nodes(
    const unordered_map<int, vector<vector<vector<int> > > >& extended_sketches)
{
    using Path_List = vector<vector<int> >;
    using Extended_Sketch = vector<Path_List>;

    int max_size_of_path_list = 0;
    for (const pair<int, Extended_Sketch>& item : extended_sketches) {
        for (const Path_List& path_list : item.second) {
            if (path_list.size() > max_size_of_path_list) {
                max_size_of_path_list = path_list.size();
            }
        }
    }

    if (max_size_of_path_list == 1) { // 上位 を避けた経路が 1 つもない場合
        return 0;
    }

    int max_number_of_avoided_top_nodes = 1;

    for (int i = 0; i < (max_size_of_path_list - 2); ++i) {
        max_number_of_avoided_top_nodes *= 2;
    }

    return max_number_of_avoided_top_nodes;
}

// 避けて経路を生成できた上位ノードの個数の最大値を返す
int get_max_number_of_avoided_top_nodes_vector_ver(
    const vector<vector<vector<vector<int> > > >& extended_sketches)
{
    using Path_List = vector<vector<int> >;
    using Extended_Sketch = vector<Path_List>;

    int max_size_of_path_list = 0;
    for (const Extended_Sketch& extended_sketch : extended_sketches) {
        for (const Path_List& path_list : extended_sketch) {
            if (path_list.size() > max_size_of_path_list) {
                max_size_of_path_list = path_list.size();
            }
        }
    }

    if (max_size_of_path_list == 1) { // 上位 を避けた経路が 1 つもない場合
        return 0;
    }

    int max_number_of_avoided_top_nodes = 1;

    for (int i = 0; i < (max_size_of_path_list - 2); ++i) {
        max_number_of_avoided_top_nodes *= 2;
    }

    return max_number_of_avoided_top_nodes;
}


// extended sketches から sketches を取得
// path_list から指定した位置の path を集める
// 指定した位置になければそれより小さい位置のものを取ってくる
unordered_map<int, vector<vector<int> > > get_sketches_from_extended_sketches(
    const unordered_map<int, vector<vector<vector<int> > > >& extended_sketches,
    int position)
{
    using Sketch = vector<vector<int> >;
    using Sketches = unordered_map<int, Sketch>;

    using Path_List = vector<vector<int> >;
    using Extended_Sketch = vector<Path_List>;

    Sketches sketches;

    for (const pair<int, Extended_Sketch>& item : extended_sketches) {
        Sketch tmp_sketch;
        for (const Path_List& path_list : item.second) {
            if (position > (path_list.size() - 1) ) {       // 指定した位置に path がない場合
                tmp_sketch.push_back( path_list.back() );
                continue;
            }

            tmp_sketch.push_back( path_list.at(position) ); // ある場合
        }

        sketches[item.first] = tmp_sketch;
    }

    return sketches;
}


// extended sketches から sketches のリストを取得
// sketches のリストは [オリジナル, 上位○個抜き, 上位○個抜き, 上位○個抜き, ...] となっている
vector<unordered_map<int, vector<vector<int> > > > get_list_of_sketches_from_extended_sketches(
    const unordered_map<int, vector<vector<vector<int> > > >& extended_sketches,
    const vector<string>& x_list)
{
    using Sketch = vector<vector<int> >;
    using Sketches = unordered_map<int, Sketch>;
    
    vector<Sketches> list_of_sketches(x_list.size());

    // 指定した path_list の位置から path を取得し sketches を生成
    // できない場合はそれより小さい位置から path を取得
    #pragma omp parallel for
    for (int i = 0; i < x_list.size(); ++i) {
        list_of_sketches[i] = get_sketches_from_extended_sketches(extended_sketches, i);
    }

    return list_of_sketches;
}


// 避けて経路を生成できた割合(少数のまま)のリストを返す
vector<double> get_avoidability_list(
    const vector<vector<vector<vector<int> > > >& extended_sketches,
    const vector<string>& x_list)
{
    using Path_List = vector<vector<int> >;
    using Extended_Sketch = vector<Path_List>;

    vector<double> avoidability_list( x_list.size() , 0);

    for (const Extended_Sketch& extended_sketch : extended_sketches) {
        for (const Path_List& path_list : extended_sketch) {
            for (int i = 0; i < path_list.size(); ++i) {
                ++avoidability_list[i];
            }
        }
    }

    // ノード数 × シードノード集合数
    double total = extended_sketches.size() * extended_sketches.front().size();
    cout << "total =" << total << endl;

    for (double& avoidability : avoidability_list) {
        avoidability /= total;
    }

    return avoidability_list;
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
bool has_cycle_for_path(const vector<int>& path)
{
    for (const int& node : path) {
        if (count(path.begin(), path.end(), node) > 1) {
            return true;
        }
    }

    return false;
}


// 最短路木 (SPT) から最短路を取得
vector<int> get_shortest_path_from_SPT(
    const vector<int>& shortest_path_tree,
    const int& source)
{
    vector<int> shortest_path;

    // sourceのpreが -1 のとき, 即ちsourceがこのSPT生成時のシードノードであるとき
    if (shortest_path_tree.at(source) == -1) {
        shortest_path.push_back(source);
        return shortest_path;
    }

    // sourceからシードノード集合まで辿る
    for (int i = source; i != -1; i = shortest_path_tree.at(i) ) {
        shortest_path.push_back(i);
    }

    return shortest_path;
}


// avoided_path_tree (APT) から上位ノードを避けた経路を取得
// 取得できない場合は空のvectorを返す
vector<int> get_avoided_path_from_APT(
    const vector<int>& avoided_path_tree,
    const int& source)
{
    vector<int> avoided_path;

    // sourceのpreが -1 のとき, 即ちsourceがこのSPT生成時のシードノードであるとき
    if (avoided_path_tree.at(source) == -1) {
        avoided_path.push_back(source);
        return avoided_path;
    }

    // sourceのpreが -2 のとき, 即ちシードノード集合への上位ノードを避けた経路がなかった場合
    if (avoided_path_tree.at(source) == -2) {
        return avoided_path;
    }

    // sourceからシードノード集合まで辿る
    for (int i = source; i != -1; i = avoided_path_tree.at(i) ) {
        avoided_path.push_back(i);
    }

    return avoided_path;
}
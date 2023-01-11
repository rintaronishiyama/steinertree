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

// sketch 生成
vector<vector<int> > sketch_index(
    const Graph& graph,
    int sketch_node,
    const vector<unordered_set<int> >& seed_node_sets)
{
    vector<vector<int> > sketch; // sketch_node の sketch : [S1との最短経路, S2との最短経路, ..., Smとの最短経路]

    /* sketch_node と Si の最短経路を探す */
    for (const unordered_set<int>& seed_node_set : seed_node_sets) {
        sketch.push_back(graph.bfs_to_node_set(sketch_node, seed_node_set));
    }


    return sketch;
}


// extended sketch 生成 (関数内での時間計測なしver)
vector<vector<vector<int> > > extended_sketch_index(
    const Graph& graph,
    int sketch_node,
    const vector<unordered_set<int> >& seed_node_sets,
    const vector<unordered_set<int> >& top_node_sets)
{
    // sketch_node の sketch : [S1との経路リスト, S2との経路リスト, ..., Smとの経路リスト]
    // Si との経路リスト : [最短経路(もともとの), 上位1個を避けた経路, 上位2個を避けた経路, ..., 上位128個を避けた経路]
    // 上位を避けた経路は生成できない場合があるが, 生成できた経路までを保持
    // 最低でも最短経路は保持することを保証
    using Path_List = vector<vector<int> >;
    using Extended_Sketch = vector<Path_List>;
    Extended_Sketch extended_sketch;


    /* sketch_node と Si の経路リストを取得 */
    for (const unordered_set<int>& seed_node_set : seed_node_sets) {
        Path_List path_list;

        // 最短経路追加
        path_list.push_back(graph.bfs_to_node_set(sketch_node, seed_node_set));

        // 可能な限り上位を避けた経路追加
        for (const unordered_set<int>& top_node_set : top_node_sets) {
            vector<int> path_avoiding_top_node
                = graph.bfs_to_node_set_avoiding_another_node_set(sketch_node, seed_node_set, top_node_set);
            
            if (path_avoiding_top_node.empty()) {
                break;
            }
            
            path_list.push_back(path_avoiding_top_node);
        }

        extended_sketch.push_back(path_list);
    }


    return extended_sketch;
}



// 各Si毎に最短路木を生成
// 最短路木を基にグラフ上の各ノードに対して, Sketch を生成
unordered_map<int, vector<vector<int> > > generate_sketches(
    const Graph& graph,
    const vector<unordered_set<int> >& seed_node_sets)
{
    using Sketch = vector<vector<int> >;
    using Sketches = unordered_map<int, Sketch>;

    Sketches sketches;

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

    #pragma omp parallel for
    for (int i = 0; i < node_list.size(); ++i) {
        Sketch sketch;
        
        for (const vector<int>& shortest_path_tree : shortest_path_tree_list) {
            vector<int> shortest_path_to_seed_node_set
                = get_shortest_path_from_SPT(shortest_path_tree, node_list.at(i));

            sketch.push_back(shortest_path_to_seed_node_set);
        }

        sketches[node_list.at(i)] = sketch;
    }

    return sketches;
}




// 各Si毎に 最短路木と avoided_path_tree (上位ノードの集合の数だけ) を生成
// それらを基に extended_sketch を生成
unordered_map<int, vector<vector<vector<int> > > > generate_extended_sketches(
    const Graph& graph,
    const vector<unordered_set<int> >& seed_node_sets,
    const vector<unordered_set<int> >& top_node_sets)
{
    using Path_List = vector<vector<int> >;     // Si の経路リスト = [最短経路, 上位1個避けた経路, ...]
    using Extended_Sketch = vector<Path_List>;  // Extended_Sketch = [S1の経路リスト, S2の経路リスト, ...]
    using Extended_Sketches = unordered_map<int, Extended_Sketch>;
    
    Extended_Sketches extended_sketches;

    // path_tree のリストをシードノード集合毎に用意
    // 外側のvector S1, S2, ...
    // 内側のvector 最短路, 上位1個避けた経路, ...
    vector<vector<vector<int> > > list_of_list_of_path_tree(seed_node_sets.size());

    #pragma omp parallel for
    for (int i = 0; i < seed_node_sets.size(); ++i) {
        vector<vector<int> > path_tree_list;

        // 最短路木を追加
        path_tree_list.push_back(graph.bfs_from_source_node_set(seed_node_sets.at(i)));

        // avoided_path_tree を追加
        for (const unordered_set<int>& top_node_set : top_node_sets) {
            path_tree_list.push_back(
                graph.bfs_from_source_node_set_avoiding_another_node_set(
                    seed_node_sets.at(i),
                    top_node_set)
            );
        }

        list_of_list_of_path_tree[i] = path_tree_list;
    }


    // 各ノードに対して extended_sketch を生成
    vector<int> node_list = graph.get_node_list();

    #pragma omp parallel for
    for (int i = 0; i < node_list.size(); ++i) {
        Extended_Sketch extended_sketch;

        for (const vector<vector<int> >& path_tree_list : list_of_list_of_path_tree) {
            Path_List path_list;

            // 最短路を追加
            path_list.push_back(get_shortest_path_from_SPT(path_tree_list.at(0), node_list.at(i)));

            // avoided_path を追加
            for (int i = 1; i < path_tree_list.size(); ++i) {
                vector<int> avoided_path = get_avoided_path_from_APT(path_tree_list.at(i), node_list.at(i));

                // avoided_path が空vector, 即ち避けた経路が見つからなかった場合
                if ( avoided_path.empty() ) {
                    break;
                }

                // 見つかれば追加
                path_list.push_back(avoided_path);
            }

            extended_sketch.push_back(path_list);
        }

        extended_sketches[node_list.at(i)] = extended_sketch;
    }

    return extended_sketches;
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
    vector<pair<pair<int, int>, vector<int> > > pair_of_terminals_and_shortest_path; // <<置き換え前ノード, 置き換え後ノード>, 前→後の最短経路>
    vector<int> alternative_terminals;
    unordered_map<int, vector<vector<int> > > alternative_sketches;

    for (int terminal : terminals) {
        if (nodes_having_sketch.count(terminal) != 0) { // sketch を持つならそのまま
            // terminal が置き換え後のターミナルリストになければ追加
            // 置き換え後のターミナルの sketch も設定
            if (find(alternative_terminals.begin(), alternative_terminals.end(), terminal) == alternative_terminals.end()) {
                alternative_terminals.push_back(terminal);
                alternative_sketches[terminal] = partial_sketches.at(terminal);
            }
        } else {
            // 近傍のsketchを持つノードを探索し置き換え
            vector<int> shortest_path = graph.bfs_to_node_set(terminal, nodes_having_sketch);
            int alternative_node = shortest_path.back();
            pair_of_terminals_and_shortest_path.push_back({ {terminal, alternative_node}, shortest_path } );
            
            // alternative_node が置き換え後のターミナルリストになければ追加
            // 置き換え後のターミナルの sketch も設定
            if (find(alternative_terminals.begin(), alternative_terminals.end(), alternative_node) == alternative_terminals.end()) {
                alternative_terminals.push_back(alternative_node);
                alternative_sketches[alternative_node] = partial_sketches.at(alternative_node);
            }
        }
    }

    // 置き換え後のターミナルに対して sketchLS
    Graph SteinerTree = sketchLS(graph, alternative_terminals, alternative_sketches);

    // 置き換え前後のターミナル間の最短経路をターミナルが ST につながるまで追加
    for (const pair<pair<int, int>, vector<int> >& item : pair_of_terminals_and_shortest_path) {
        add_edges_til_terminal_connects_to_ST(SteinerTree, item.second);
    }

    // 閉路がないか確認
    if ( SteinerTree.has_cycle() ) {
        cout << "Steiner Tree has cycle." << endl;
        throw;
    }

    // ST に余計な経路があれば削除
    remove_unnecessary_path_from_ST(SteinerTree, terminals);

    return SteinerTree;
}






/* extended sketches を扱う関数 */
// 避けて経路を生成できた上位ノードの個数の最大値を返す
int get_max_number_of_avoided_top_nodes(
    const unordered_map<int, vector<vector<vector<int> > > >& extended_sketches)
{
    using Path_List = vector<vector<int> >;
    using Extended_Sketch = vector<Path_List>;

    int max_number_of_avoided_top_nodes = 1;

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
// sketches のリストは [オリジナル, 上位1個抜き, 上位2個抜き, 上位4個抜き, ...] となっている
vector<unordered_map<int, vector<vector<int> > > > get_list_of_sketches_from_extended_sketches(
    const unordered_map<int, vector<vector<vector<int> > > >& extended_sketches,
    int max_number_of_avoided_top_nodes)
{
    using Sketch = vector<vector<int> >;
    using Sketches = unordered_map<int, Sketch>;
    
    vector<Sketches> list_of_sketches;

    // path_list のサイズの最大値を計算
    int max_size_of_path_list = 1;
    if (max_number_of_avoided_top_nodes != 0) {
        int tmp = 1;
        while (tmp <= max_number_of_avoided_top_nodes) {
            tmp *= 2;
            ++max_size_of_path_list;
        }
    }

    // 指定した path_list の位置から path を取得し sketches を生成
    // できない場合はそれより小さい位置から path を取得
    for (int i = 0; i < max_size_of_path_list; ++i) {
        list_of_sketches.push_back(
            get_sketches_from_extended_sketches(extended_sketches, i)
        );
    }

    return list_of_sketches;
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


// 非ターミナルの葉のリストを返す
vector<int> get_non_terminal_leaves (
    const unordered_map<int, vector<int> >& adjacency_list,
    const unordered_set<int>& terminal_set)
{
    vector<int> non_terminal_leaves;

    for (const pair<int, vector<int> >& item : adjacency_list) {
        // 次数が 1 のノードは葉として追加
        if ( ( item.second.size() == 1 ) && ( !terminal_set.count(item.first) )) {
            non_terminal_leaves.push_back(item.first);
        }
    }

    return non_terminal_leaves;
}


// ターミナルを含まない枝のリストを返す
vector<vector<int> > get_branches_without_terminal(
    const unordered_map<int, vector<int> >& adjacency_list,
    const unordered_set<int>& terminal_set,
    const vector<int>& non_terminal_leaves)
{
    vector<vector<int> > branches_without_terminal;

    for (const int& non_terminal_leaf : non_terminal_leaves) {
        // 現在頂点
        int current = non_terminal_leaf;
        
        // 1 つ前の頂点
        int previous = -1;

        // 削除する経路
        vector<int> branch_without_terminal;

        // ターミナルに途中でヒットするかのフラグ
        bool hit_terminal = false;

        while (true) {
            // 次数が 3 以上のノードに当たれば終了. 後に追加
            if (adjacency_list.at(current).size() > 2) {
                branch_without_terminal.push_back(current);
                break;
            }

            // 途中でターミナルに当たれば終了. 追加はしない
            if ( terminal_set.count(current) ) {
                hit_terminal = true;
                break;
            }

            branch_without_terminal.push_back(current);

            // 次の頂点を決定
            for (const int& node : adjacency_list.at(current) ) {
                if (node != previous) {
                    previous = current;
                    current = node;
                    break; // 次数 2 のノードしか見ないので, 見つかった時点で終了
                }
            }
        }

        // ターミナルにヒットしていなければ追加
        if (!hit_terminal) {
            branches_without_terminal.push_back(branch_without_terminal);
        }
    }

    return branches_without_terminal;
}


// ターミナルを含むが, 葉がターミナルでない枝上で
// 葉からターミナルまでのパスのリストを返す
vector<vector<int> > get_paths_leaf_to_terminal(
    const unordered_map<int, vector<int> >& adjacency_list,
    const unordered_set<int>& terminal_set,
    const vector<int>& non_terminal_leaves)
{
    vector<vector<int> > paths_leaf_to_terminal;

    for (const int& non_terminal_leaf : non_terminal_leaves) {
        // 現在頂点
        int current = non_terminal_leaf;
        // 1 つ前の頂点
        int previous = -1;

        // 削除する経路
        vector<int> path_leaf_to_terminal;


        while (true) {
            // 次数が 3 以上のノードに当たったら異常終了
            if (adjacency_list.at(current).size() > 2) {
                cout << "Branch without terminal is left. It should be deleted before this." << endl;
                throw;
            }


            // 途中でターミナルに当たれば終了. 後に追加
            if ( terminal_set.count(current) ) {
                path_leaf_to_terminal.push_back(current);
                break;
            }

            path_leaf_to_terminal.push_back(current);

            // 次の頂点を決定
            for (const int& node : adjacency_list.at(current) ) {
                if (node != previous) {
                    previous = current;
                    current = node;
                    break; // 次数 2 のノードしか見ないので, 見つかった時点で終了
                }
            }
        }

        paths_leaf_to_terminal.push_back(path_leaf_to_terminal);
    }

    return paths_leaf_to_terminal;
}


// ST の葉が全てターミナルとなるように余計な経路を削除
// 余計な経路の種類
// 1 : ターミナルを含まない枝 (除くのは葉から枝分かれした位置までの経路)
// 2 : ターミナルを含むが, 葉がターミナルでない枝 (除くのは葉から枝中のターミナルまでの経路)
void remove_unnecessary_path_from_ST(Graph& ST, vector<int> terminals)
{
    // 扱いやすくするため unordered_set にコンバート
    unordered_set<int> terminal_set;
    for (const int& terminal : terminals) {
        terminal_set.insert(terminal);
    }

    // 隣接リストを用意
    const unordered_map<int, vector<int> >& adjacency_list = ST.get_adjacency_list();

    // ST 中のターミナルでない葉
    vector<int> non_terminal_leaves;

    // 余計な経路タイプ 1 がなくなるまで削除
    while (true) {
        non_terminal_leaves = get_non_terminal_leaves(adjacency_list, terminal_set);

        // 余計な経路のタイプ 1 を特定
        vector<vector<int> > branches_without_terminal
            = get_branches_without_terminal(adjacency_list, terminal_set, non_terminal_leaves);

        // 見つからなければ終了
        if ( branches_without_terminal.empty() ) {
            break;
        }

        // 削除
        for (const vector<int>& branch_without_terminal : branches_without_terminal) {
            ST.delete_path(branch_without_terminal);
        }

        non_terminal_leaves.clear();
    }

    // ST 中のターミナルでない葉を再び特定
    non_terminal_leaves.clear();
    non_terminal_leaves = get_non_terminal_leaves(adjacency_list, terminal_set);

    // 余計な経路のタイプ 2 を特定
    vector<vector<int> > paths_leaf_to_terminal
        = get_paths_leaf_to_terminal(adjacency_list, terminal_set, non_terminal_leaves);

    // 削除
    for (const vector<int>& path_leaf_to_terminal : paths_leaf_to_terminal) {
        ST.delete_path(path_leaf_to_terminal);
    }
}


void add_edges_til_terminal_connects_to_ST(Graph& ST, const vector<int>& path){
    int terminal = path.front();
    
    // すでに ST 内にないか確認
    vector<int> node_list = ST.get_node_list();
    if ( find(node_list.begin(), node_list.end(), terminal) != node_list.end() ) {
        return;
    }

    // ターミナルがつながるまでエッジを追加
    for (int i = 0; i < (path.size() - 1); ++i) {
        ST.add_edge(path.at(i), path.at(i + 1));

        // つながったら終了
        if ( ST.is_connected() ) {
            return;
        }
    }
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
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <utility>
#include "graph.h"
#include "read.h"
#include "random.h"
#include "sketchLS.h"
#include "write.h"
#include "divide.h"
#include "evaluate.h"
#include "x.h"

// Files to compile
// main.cpp graph.cpp read.cpp split.cpp sketchLS.cpp write.cpp random.cpp divide.cpp evaluate.cpp x.cpp

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::unordered_map;
using std::pair;
using std::to_string;

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    /* グラフの選択 */
    string graph_name;
    cout << "Enter graph name : ";
    cin >> graph_name;


    /* グラフのデータセットがあるか確認 */
    string dataset_path = "../dataset/" + graph_name + ".txt";
    if ( !fs::is_regular_file(dataset_path) ) { // なければ異常終了
        cout << "There is no such dataset" << endl;
        return 1;
    }


    /* グラフ読み込み */
    Graph graph;
    read_graph_from_txt_file(dataset_path, graph);
    cout << "Complete reading graph" << endl;
    

    /* extended_sketches が生成済みか確認 */
    string result_dir_path =  "./" + graph_name;
    string extended_sketches_path;
    extended_sketches_path = result_dir_path + "/extended_sketches.txt";
    if ( !fs::is_regular_file(extended_sketches_path) ) {
        cout << "There is no sketches.txt" << endl;
        return 1;
    }


    /* extended_sketches 読み込み */
    // 型エイリアス
    using Path_List = vector<vector<int> >;
    using Extended_Sketch = vector<Path_List>;
    using Extended_Sketches = unordered_map<int, Extended_Sketch>;

    Extended_Sketches extended_sketches;
    read_extended_sketches_from_txt_file(extended_sketches_path, extended_sketches);
    cout << "Complete reading extended sketches" << endl;


    /* ターミナルファイルが生成済みか確認 */
    string terminals_path = result_dir_path + "/terminals.txt";
    if ( !fs::is_regular_file(terminals_path) ) {
        cout << "There is no terminals.txt" << endl;
        return 1;
    }


    /* ターミナルを読み込み */
    vector<vector<int> > list_of_terminals;
    read_list_of_terminals_from_txt_file(terminals_path, list_of_terminals);
    cout << "Complete reading terminals" << endl;


    /* extended sketches から sketches のリストを取得 */
    using Sketch = vector<vector<int> >;
    using Sketches = unordered_map<int, Sketch>;
    
    // 避けて経路を生成できた bc 上位の個数の最大
    int max_number_of_avoided_bc_top_nodes = get_max_number_of_avoided_bc_top_nodes(extended_sketches);
    cout << "max number of avoided bc top nodes : " << max_number_of_avoided_bc_top_nodes << endl;
    
    vector<Sketches> list_of_avoided_bc_sketches
        = get_list_of_sketches_from_extended_sketches(extended_sketches, max_number_of_avoided_bc_top_nodes);
    cout << "Complete getting list of avoided bc sketches" << endl;


    /* extended sketches を分割 */
    // 分割する長さを決定
    double length_to_divide_sketches = 0.05;
    // cout << "length to divide sketches (e.g., 0.05) : ";
    // cin >> length_to_divide_sketches;

    // 次数の降順にソートしたノードリストを取得
    vector<int> node_list_sorted_by_degree = graph.get_node_list_sorted_by_degree();

    // list_of_sketches 内の Sketches を分割
    // 外側の vector : 避けたbc上位ノードの個数             [original, 1, 2, 4, ...]
    // 内側の vector : sketch の保持を限定したノードの範囲   [no division, 0-5, 5-10, ...]
    // 0-5 以降は partial sketches
    vector<vector<Sketches> > list_of_list_of_sketches;

    for (const Sketches& sketches : list_of_avoided_bc_sketches) {
        vector<Sketches> tmp_list_of_sketches;

        // no division を最初に追加
        tmp_list_of_sketches.push_back(sketches);

        // 分割した partial_sketches を追加
        vector<Sketches> tmp_list_of_partial_sketches
            = divide_sketches(sketches, length_to_divide_sketches, node_list_sorted_by_degree);
        
        tmp_list_of_sketches.insert(
            tmp_list_of_sketches.end(),
            tmp_list_of_partial_sketches.begin(),
            tmp_list_of_partial_sketches.end()
        );

        list_of_list_of_sketches.push_back(tmp_list_of_sketches);
    }
    cout << "Complete dividing extended sketches" << endl;


    /* x軸の値のリストを取得 */
    vector<string> x_list_for_avoided_bc_top_nodes
        = get_x_list_for_avoided_bc_top_nodes(max_number_of_avoided_bc_top_nodes);

    vector<string> x_list_for_limit_range
        = get_x_list_for_limit_range(length_to_divide_sketches);
    cout << "Complete getting x list" << endl;


    /* 評価の値のリスト */
    // 外側の vector : 避けたbc上位ノードの個数             [original, 1, 2, 4, ...]
    // 内側の vector : sketch の保持を限定したノードの範囲   [no division, 0-5, 5-10, ...]
    vector<vector<double> > list_of_list_of_overlap_ratio(
        x_list_for_avoided_bc_top_nodes.size(),
        vector<double>(x_list_for_limit_range.size(), 0)
    );
    vector<vector<double> > list_of_list_of_ST_size(
        x_list_for_avoided_bc_top_nodes.size(),
        vector<double>(x_list_for_limit_range.size(), 0)
    );
    vector<vector<double> > list_of_list_of_sum_of_degree(
        x_list_for_avoided_bc_top_nodes.size(),
        vector<double>(x_list_for_limit_range.size(), 0)
    );
    vector<vector<double> > list_of_list_of_sum_of_bc(
        x_list_for_avoided_bc_top_nodes.size(),
        vector<double>(x_list_for_limit_range.size(), 0)
    );

    // BC の読み込み
    unordered_map<int, double> bc_map;
    string BC_txt_path = result_dir_path + "/BC.txt";
    read_bc_from_txt_file(BC_txt_path, bc_map);


    /* ST 確認の為の準備 */ 
    // partial sketchLS の ST の葉がターミナルだけか確認のため
    vector<Graph> list_of_ST_for_checking;
    vector<vector<int> > list_of_terminals_for_checking;




    /* 実行 */




    // 何セット目の実行か確認
    int count_terminals = 0;

    // グラフのノード数を取得
    int n = graph.get_number_of_nodes();

    for (const vector<int>& terminals : list_of_terminals) {
        ++count_terminals;
        cout << "set " << count_terminals << " starts" << endl;

        // 外側の vector : 避けたbc上位ノードの個数             [original, 1, 2, 4, ...]
        // 内側の vector : sketch の保持を限定したノードの範囲   [no division, 0-5, 5-10, ...]
        vector<vector<Graph> > list_of_list_of_ST;

        // sketchLS or partial_sketchLS 実行
        for (const vector<Sketches>& list_of_sketches : list_of_list_of_sketches) {
            vector<Graph> tmp_list_of_ST;

            for (const Sketches& sketches : list_of_sketches) {
                // 全ノードの Sketch を持つ sketches は sketchLS
                if (sketches.size() == n) {
                    tmp_list_of_ST.push_back(sketchLS(graph, terminals, sketches));
                    continue;
                }

                // 全ノードの Sketch を持たない sketches は partial_sketchLS
                tmp_list_of_ST.push_back(partial_sketchLS(graph, terminals, sketches));
            }

            list_of_list_of_ST.push_back(tmp_list_of_ST);
        }

        // overlap ratio 記録
        const Graph& original_ST = list_of_list_of_ST.front().front();
        for (int i = 0; i < list_of_list_of_ST.size(); ++i) {
            for (int j = 0; j < list_of_list_of_ST.at(i).size(); ++j) {
                list_of_list_of_overlap_ratio[i][j]
                    += evaluate_overlap_ratio(original_ST, list_of_list_of_ST[i][j], terminals);
            }
        }

        // サイズ記録
        for (int i = 0; i < list_of_list_of_ST.size(); ++i) {
            for (int j = 0; j < list_of_list_of_ST.at(i).size(); ++j) {
                list_of_list_of_ST_size[i][j]
                    += list_of_list_of_ST[i][j].get_number_of_edges();
            }
        }

        // ターミナルを除くノードの次数の合計を記録
        for (int i = 0; i < list_of_list_of_ST.size(); ++i) {
            for (int j = 0; j < list_of_list_of_ST.at(i).size(); ++j) {
                list_of_list_of_sum_of_degree[i][j]
                    += evaluate_sum_of_degree(graph, list_of_list_of_ST[i][j], terminals);
            }
        }

        // ターミナルを除くノードのBCの合計を記録
        for (int i = 0; i < list_of_list_of_ST.size(); ++i) {
            for (int j = 0; j < list_of_list_of_ST.at(i).size(); ++j) {
                list_of_list_of_sum_of_bc[i][j]
                    += evaluate_sum_of_bc(bc_map, list_of_list_of_ST[i][j], terminals);
            }
        }

        // ST 確認のため
        if (count_terminals % (list_of_terminals.size() / 10) == 0) {
            // 最もbc上位を避けて, 次数下位に sketch をもたせた場合の ST をチェック
            list_of_ST_for_checking.push_back(list_of_list_of_ST.back().back());
            list_of_terminals_for_checking.push_back(terminals);
        }


    }
    cout << "Complete executing" << endl;


    /* 平均化 */
    for (vector<double>& list_of_overlap_ratio : list_of_list_of_overlap_ratio) {
        for (double& overlap_ratio : list_of_overlap_ratio) {
            overlap_ratio /= list_of_terminals.size();
        }
    }

    for (vector<double>& list_of_ST_size : list_of_list_of_ST_size) {
        for (double& ST_size : list_of_ST_size) {
            ST_size /= list_of_terminals.size();
        }
    }

    for (vector<double>& list_of_sum_of_degree : list_of_list_of_sum_of_degree) {
        for (double& sum_of_degree : list_of_sum_of_degree) {
            sum_of_degree /= list_of_terminals.size();
        }
    }

    for (vector<double>& list_of_sum_of_bc : list_of_list_of_sum_of_bc) {
        for (double& sum_of_bc : list_of_sum_of_bc) {
            sum_of_bc /= list_of_terminals.size();
        }
    }
    cout << "Complete averaging" << endl;


    /* 評価を保存 */
    string overlap_ratio_path = result_dir_path + "/overlap_ratio.txt";
    string ST_size_path = result_dir_path + "/size.txt";
    string sum_of_degree_path = result_dir_path + "/sum_of_degree.txt";
    string sum_of_bc_path = result_dir_path + "/sum_of_bc.txt";

    write_overlap_ratio(
        overlap_ratio_path,
        x_list_for_avoided_bc_top_nodes,
        x_list_for_limit_range,
        list_of_list_of_overlap_ratio);
    
    write_ST_size(
        ST_size_path,
        x_list_for_avoided_bc_top_nodes,
        x_list_for_limit_range,
        list_of_list_of_ST_size);

    write_sum_of_degree(
        sum_of_degree_path,
        x_list_for_avoided_bc_top_nodes,
        x_list_for_limit_range,
        list_of_list_of_sum_of_degree);

    write_sum_of_bc(
        sum_of_bc_path,
        x_list_for_avoided_bc_top_nodes,
        x_list_for_limit_range,
        list_of_list_of_sum_of_bc);

    cout << "Complete writing evaluation" << endl;


    // ST 確認の為に保存
    string ST_checking_path = result_dir_path + "/ST_checking";
    fs::create_directories(ST_checking_path);
    int count_for_ST_checking = 1;
    for (int i = 0; i < list_of_ST_for_checking.size(); ++i) {
        const Graph& ST = list_of_ST_for_checking.at(i);
        const vector<int>& terminals = list_of_terminals_for_checking.at(i);

        string ST_path = ST_checking_path + "/ST" + to_string(count_for_ST_checking) + ".txt";

        write_graph(ST_path, ST);
        write_terminals_to_exisiting_txt(ST_path, terminals);

        ++count_for_ST_checking;
    }
    cout << "Complete writing STs for checking" << endl;

    return 0;
}
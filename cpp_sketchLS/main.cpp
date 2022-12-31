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
    string extended_sketches_path = result_dir_path + "/extended_sketches.txt";
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



    /* extended_sketches を分割なしで実行*/
    
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
    
    vector<Sketches> list_of_Sketches
        = get_list_of_sketches_from_extended_sketches(extended_sketches, max_number_of_avoided_bc_top_nodes);
    cout << "Complete getting list of sketches" << endl;


    /* x軸の値のリスト取得 */
    vector<string> x_list_for_list_of_sketches = get_x_list_for_list_of_sketches(max_number_of_avoided_bc_top_nodes);
    cout << "Complete getting x list" << endl;


    /* 評価の値のリスト作成 */
    vector<double> overlap_ratio_list(list_of_Sketches.size() - 1, 0);
    vector<double> ST_size_list(list_of_Sketches.size(), 0);


    /* 実行 */
    for (const vector<int>& terminals : list_of_terminals) {
        vector<Graph> STs;
        
        // sketchLS 実行
        for (const Sketches& sketches : list_of_Sketches) {
            STs.push_back( sketchLS(graph, terminals, sketches) );
        }

        // overlap ratio 記録
        const Graph& original_ST = STs.front();
        for (int i = 1; i < STs.size(); ++i) {
            overlap_ratio_list[i - 1] += evaluate_overlap_ratio(original_ST, STs[i], terminals);
        }

        // サイズ記録
        for (int i = 0; i < STs.size(); ++i) {
            ST_size_list[i] += STs.at(i).get_number_of_edges();
        }
    }
    cout << "Complete executing" << endl;


    /* 平均化 */
    // 試行回数 (list_of_terminalsの大きさ) で割る
    for (double& overlap_ratio : overlap_ratio_list) {
        overlap_ratio /= list_of_terminals.size(); 
    }
    for (double& size : ST_size_list) {
        size /= list_of_terminals.size();
    }
    cout << "Complete averaging" << endl;


    /* 評価を保存 */
    string overlap_ratio_path = result_dir_path + "/overlap_ratio.txt";;
    string ST_size_path = result_dir_path + "/size.txt";;
    write_overlap_ratio(overlap_ratio_path, x_list_for_list_of_sketches, overlap_ratio_list);
    write_ST_size(ST_size_path, x_list_for_list_of_sketches, ST_size_list);
    cout << "Complete writing results" << endl;


    return 0;
}
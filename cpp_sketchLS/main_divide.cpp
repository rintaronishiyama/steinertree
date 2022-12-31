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

// Files to compile
// main_divide.cpp graph.cpp read.cpp split.cpp sketchLS.cpp write.cpp random.cpp divide.cpp evaluate.cpp

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



    /* extended_sketches を分割ありで実行*/

    /* extended sketches を分割 */
    double length_to_divide = 0.05;
    
    // sketch 保持の範囲をx軸用に用意
    vector<pair<double, double> > sketches_range_list;
    double count = 0;
    while (count < 1) {
        double bottom = count * 100;
        double top = (count + length_to_divide) * 100;
        cout << top << endl;
        sketches_range_list.push_back({bottom, top});
        count += length_to_divide;
    }

    vector<int> node_list_sorted_by_degree = graph.get_node_list_sorted_by_degree();
    vector<Sketches> partial_sketches_list = divide_sketches(sketches, length_to_divide, node_list_sorted_by_degree);

    vector<double> overlap_ratio_list(sketches_range_list.size(), 0);  // もとの sketchLS のターミナルを除くノード以外をどれだけ含むか
    vector<double> ST_size_list(sketches_range_list.size() + 1, 0);

    // 1000 回実行
    for (int i = 0; i < 1000; ++i) {
        /* ターミナルの決定 */
        int size_of_terminals = 5;
        vector<int> terminals = decide_terminals(graph, size_of_terminals);


        /* sketchLS 実行 */
        Graph steiner_tree_of_sketchLS = sketchLS(graph, terminals, true_sketches);
        cout << "Complete executing SketchLS" << endl;


        /* partial_sketchLS 実行 */
        vector<Graph> steiner_trees_of_partial_sketchLS;
        for (Sketches partial_sketches : partial_sketches_list) {
            steiner_trees_of_partial_sketchLS.push_back( partial_sketchLS(graph, terminals, partial_sketches) );
        }


        /* overlap ratio 評価 */
        for (int j = 0; j < steiner_trees_of_partial_sketchLS.size(); ++j) {
            overlap_ratio_list[j] += evaluate_overlap_ratio(steiner_tree_of_sketchLS, steiner_trees_of_partial_sketchLS[i], terminals);
        }

        /* サイズ記録 */
        ST_size_list[0] += steiner_tree_of_sketchLS.get_number_of_edges();
        for (int j = 0; j < steiner_trees_of_partial_sketchLS.size(); ++j) {
            ST_size_list[j + 1] += steiner_trees_of_partial_sketchLS[j].get_number_of_edges();
        }
    }


    /* 平均化 */
    for (double& overlap_ratio : overlap_ratio_list) {
        overlap_ratio /= 1000;
    }
    for (double& size : ST_size_list) {
        size /= 1000;
    }


    /* 評価を保存 */
    string overlap_ratio_path = result_dir_path + "/overlap_ratio.txt";;
    string ST_size_path = result_dir_path + "/size.txt";;
    write_overlap_ratio(overlap_ratio_path, sketches_range_list, overlap_ratio_list);
    write_ST_size(ST_size_path, sketches_range_list, ST_size_list);

    return 0;
}
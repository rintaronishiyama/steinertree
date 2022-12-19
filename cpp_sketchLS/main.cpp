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
#include "output.h"
#include "divide.h"
#include "evaluate.h"

// Files to compile
// main.cpp graph.cpp read.cpp split.cpp sketchLS.cpp write.cpp random.cpp output.cpp divide.cpp evaluate.cpp

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


    /* sketches が生成済みか確認 */
    string result_dir_path =  "./" + graph_name;
    string sketches_path = result_dir_path + "/sketches.txt";
    if ( !fs::is_regular_file(sketches_path) ) {
        cout << "There is no sketches.txt" << endl;
        return 1;
    }


    /* sketches 読み込み */
    using Sketches = unordered_map<int, vector<vector<int> > > ;
    Sketches sketches;
    read_sketches_from_txt_file(sketches_path, sketches);
    cout << "Complete reading sketches" << endl;


    /* Sketches を分割 */
    double length_to_divide = 0.05;
    
    // sketch 保持の範囲をx軸用に用意
    vector<pair<double, double> > sketches_range_list;
    double tmp = 0;
    while (tmp < 1) {
        double bottom = tmp * 100;
        double top = (tmp + length_to_divide) * 100;
        cout << top << endl;
        sketches_range_list.push_back({bottom, top});
        tmp += length_to_divide;
    }

    vector<int> node_list_sorted_by_degree = graph.get_node_list_sorted_by_degree();
    vector<Sketches> partial_sketches_list = divide_sketches_by_length_to_divide(sketches, length_to_divide, node_list_sorted_by_degree);

    vector<double> overlap_ratio_list(sketches_range_list.size(), 0);                              // もとの sketchLS のターミナルを除くノード以外をどれだけ含むか
    vector<pair<int, double> > diameter_and_number_of_unique_node;  // ターミナルの直径とユニークなノード数

    // 1000 回実行
    for (int i = 0; i < 1000; ++i) {
        /* ターミナルの決定 */
        int size_of_terminals = 5;
        vector<int> terminals = decide_terminals(graph, size_of_terminals);
        cout << "Complete deciding terminals" << endl;
        output_terminals(terminals);


        /* sketchLS 実行 */
        Graph steiner_tree_of_sketchLS = sketchLS(graph, terminals, sketches);
        cout << "Complete executing SketchLS" << endl;


        /* partial_sketchLS 実行 */
        vector<Graph> steiner_trees_of_partial_sketchLS;
        for (Sketches partial_sketches : partial_sketches_list) {
            steiner_trees_of_partial_sketchLS.push_back( partial_sketchLS(graph, terminals, partial_sketches) );
        }


        /* overlap ratio 評価 */
        for (int i = 0; i < steiner_trees_of_partial_sketchLS.size(); ++i) {
            overlap_ratio_list[i] += evaluate_overlap_ratio(steiner_tree_of_sketchLS, steiner_trees_of_partial_sketchLS[i], terminals);
        }
    }


    /* 平均化 */
    for (double& overlap_ratio : overlap_ratio_list) {
        overlap_ratio /= 1000;
    }


    /* 評価を保存 */
    string overlap_ratio_path = result_dir_path + "/overlap_ratio.txt";
    write_overlap_ratio(overlap_ratio_path, sketches_range_list, overlap_ratio_list);

    return 0;
}
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include "graph.h"
#include "read.h"
#include "random.h"
#include "sketchLS.h"
#include "write.h"
#include "output.h"

// Files to compile
// main.cpp graph.cpp read.cpp split.cpp sketchLS.cpp write.cpp random.cpp output.cpp

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::unordered_map;

namespace fs = std::filesystem;

// debug


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
    unordered_map<int, vector<vector<int> > > sketches;
    read_sketches_from_txt_file(sketches_path, sketches);
    cout << "Complete reading sketches" << endl;


    /* ターミナルの決定 */
    int size_of_terminals = 5;
    vector<int> terminals = decide_terminals(graph, size_of_terminals);
    cout << "Complete deciding terminals" << endl;
    output_terminals(terminals);


    /* sketchLS 実行 */
    Graph steiner_tree = sketchLS(graph, terminals, sketches);
    cout << "Complete executing SketchLS" << endl;


    /* グラフを保存 */
    string write_graph_path = result_dir_path + "/SteinerTree.txt";
    write_graph(write_graph_path, steiner_tree);
    cout << "Complete writing graph" << endl;
    
    return 0;
}
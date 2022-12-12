#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "graph.h"
#include "read.h"
#include "random.h"
#include "sketchLS.h"
#include "write.h"

// Files to compile
// main.cpp graph.cpp read.cpp split.cpp sketchLS.cpp write.cpp random.cpp

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::ifstream;
using std::unordered_map;

// debug


int main(int argc, char* argv[])
{
    /* グラフ読み込み */
    Graph graph;
    string dataset_path = "../dataset/test_dataset.txt";
    read_graph_from_txt_file(dataset_path, graph);

    /* sketches 読み込み */
    unordered_map<int, vector<vector<int> > > sketches;
    string read_file_path = "sketches.txt";
    read_sketches_from_txt_file(read_file_path, sketches);

    /* ターミナルの決定 */
    int size_of_terminals = 5;
    vector<int> terminals = decide_terminals(graph, size_of_terminals);

    /* sketchLS 実行 */
    Graph steiner_tree = sketchLS(graph, terminals, sketches);

    /* グラフを保存 */
    string write_file_path = "graph.txt";
    write_graph(write_file_path, steiner_tree);
    
    return 0;
}
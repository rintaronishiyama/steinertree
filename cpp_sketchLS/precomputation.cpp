#include <vector>
#include <random>
#include <iterator>  // std::back_inserter
#include <algorithm> // std::sample
#include <string>
#include <unordered_map>
#include <iostream>
#include "graph.h"
#include "read.h"
#include "sketchLS.h"
#include "write.h"

// Files to compile
// precomputation.cpp graph.cpp read.cpp split.cpp sketchLS.cpp write.cpp

using std::vector;
using std::sample;
using std::back_inserter;
using std::string;
using std::unordered_map;
using std::cout;
using std::endl;

// 乱数の準備
std::random_device rd;
std::mt19937 engine(rd());

// 前方宣言
vector<vector<int> > decide_seed_node_sets(const Graph& graph); 

int main(int argc, char* argv[])
{
    /* グラフ読み込み */
    Graph graph;
    string dataset_path = "../dataset/test_dataset.txt";
    read_graph_from_txt_file(dataset_path, graph);
    
    // 連結かどうか確認
    if ( !graph.is_connected() ) {
        cout << "This graph is not connected" << endl;
        return 1;
    }

    /* シードノード集合の決定 */
    vector<vector<int> > seed_node_sets = decide_seed_node_sets(graph);

    /* 次数の降順にソートしたノードのリストを用意 */
    vector<int> node_list_sort_by_degree = graph.get_node_list_sorted_by_degree();

    /* 次数の降順に sketch 生成 */
    unordered_map<int, vector <vector<int> > > sketches;
    for (const int& node : node_list_sort_by_degree) {
        sketches[node] = ( sketch_index(graph, node, seed_node_sets) );
    }
    
    /* sketches 保存 */
    string write_file_path = "sketches.txt";
    write_sketches(write_file_path, sketches);

    return 0;
}

vector<vector<int> > decide_seed_node_sets(const Graph& graph) {
    // シードノード集合のvector [S1, S2, ..., Sm]
    vector<vector<int> > seed_node_sets;

    const vector<int>& node_list = graph.get_node_list();
    int n = graph.get_number_of_nodes();
    
    // サイズの異なるシードノード集合を追加
    int size = 1;
    while ( size < (n / 2) ) {
        vector<int> tmp;
        // node_list から size 分サンプリング
        sample(
            node_list.begin(),
            node_list.end(),
            back_inserter(tmp),
            size,
            engine
        );
        seed_node_sets.push_back(tmp);
        size *= 2;
    }

    return seed_node_sets;
}


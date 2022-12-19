#include <vector>
#include <iterator>  // std::back_inserter
#include <algorithm> // std::sample
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <filesystem>
#include "graph.h"
#include "read.h"
#include "sketchLS.h"
#include "write.h"
#include "random.h"

// Files to compile
// precomputation.cpp graph.cpp read.cpp split.cpp sketchLS.cpp write.cpp random.cpp

using std::vector;
using std::sample;
using std::back_inserter;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::cin;
using std::cout;
using std::endl;


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
    // 連結かどうか確認
    if ( !graph.is_connected() ) {
        cout << "This graph is not connected" << endl;
        return 1;
    }


    /* グラフの情報を表示 */
    cout << "This graph has " << graph.get_number_of_nodes()
        << " nodes and " << graph.get_number_of_edges() << "edges" << endl;


    /* sketches が生成済みか確認 */
    string result_dir_path =  "./" + graph_name;
    string sketches_path = result_dir_path + "/sketches.txt";
    if ( fs::is_regular_file(sketches_path) ) {
        cout << "There is already sketches.txt" << endl;
        return 1;
    }


    /* シードノード集合の決定 */
    vector<unordered_set<int> > seed_node_sets = decide_seed_node_sets(graph);
    cout << "Complete deciding seed node sets" << endl;


    /* 次数の降順にソートしたノードのリストを用意 */
    vector<int> node_list_sort_by_degree = graph.get_node_list_sorted_by_degree();
    cout << "Complete sorting by degree" << endl;
    
    
    /* 次数の降順に sketch 生成 */
    unordered_map<int, vector <vector<int> > > sketches;
    #pragma omp parallel for
    for (int i = 0; i < node_list_sort_by_degree.size(); ++i) {
        sketches[node_list_sort_by_degree[i]] = ( sketch_index(graph, node_list_sort_by_degree[i], seed_node_sets) );
        cout << "Sketch " << node_list_sort_by_degree[i] << " done" << endl;
    }
    
    /* sketches 保存 */
    fs::create_directories(result_dir_path);
    string write_sketches_path = result_dir_path + "/sketches.txt";
    write_sketches(write_sketches_path, sketches);

    return 0;
}

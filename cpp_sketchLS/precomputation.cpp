#include <vector>
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
// precomputation.cpp graph.cpp read.cpp split.cpp sketchLS.cpp write.cpp random.cpp

using std::vector;
using std::sample;
using std::back_inserter;
using std::string;
using std::unordered_map;
using std::cout;
using std::endl;

// 前方宣言
vector<vector<int> > decide_seed_node_sets(const Graph& graph); 

int main(int argc, char* argv[])
{
    /* グラフ読み込み */
    Graph graph;
    string dataset_path = "../dataset/ego-facebook.txt";
    read_graph_from_txt_file(dataset_path, graph);
    cout << "Complete reading graph" << endl;
    // 連結かどうか確認
    if ( !graph.is_connected() ) {
        cout << "This graph is not connected" << endl;
        return 1;
    }


    /* シードノード集合の決定 */
    vector<vector<int> > seed_node_sets = decide_seed_node_sets(graph);
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
    string write_file_path = "sketches.txt";
    write_sketches(write_file_path, sketches);

    return 0;
}

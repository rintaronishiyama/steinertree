#include <vector>
#include <iterator>  // std::back_inserter
#include <algorithm> // std::sample, std::sort
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <utility>   // std::pair
#include <math.h>    // std::ceil
#include "graph.h"
#include "read.h"
#include "sketchLS.h"
#include "write.h"
#include "random.h"
#include "divide.h"
#include "set.h"

// Files to compile
// precomputation.cpp graph.cpp read.cpp split.cpp sketchLS.cpp write.cpp random.cpp divide.cpp set.cpp

using std::vector;
using std::sample;
using std::back_inserter;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::cin;
using std::cout;
using std::endl;
using std::pair;
using std::map;
using std::ceil;
using std::sort;

namespace fs = std::filesystem;
namespace ch = std::chrono;

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


    /* シードノード集合の決定 */
    vector<unordered_set<int> > seed_node_sets = decide_seed_node_sets(graph);
    cout << "Complete deciding seed node sets" << endl;


    /* 次数の降順にソートしたノードのリストを用意 */
    vector<int> node_list_sorted_by_degree = graph.get_node_list_sorted_by_degree();
    cout << "Complete sorting by degree" << endl;
    

    /* ノードリストを 0.1% 毎に区切る */
    double length_to_divide = 0.001;
    vector<vector<int> > divided_list_of_node_list_sorted_by_degree 
        = divide_node_list(node_list_sorted_by_degree, length_to_divide);
    cout << "Complete dividing node list" << endl;


    /* BC.txtを読み込み */
    string BC_txt_path = graph_name + "/BC.txt";
    vector<int> node_list_sorted_by_bc;
    read_bc_from_txt_file(BC_txt_path, node_list_sorted_by_bc);
    cout << "Complete reading BC" << endl;


    /* BC上位ノード集合のリストを取得 */
    int max_size_of_bc_top_node_set = 128;
    vector<unordered_set<int> > bc_top_node_sets
        = get_node_sets_from_node_list(node_list_sorted_by_bc, max_size_of_bc_top_node_set); 
    cout << "Complete getting BC top node sets" << endl;


    /* 次数の降順に sketch 生成 */


    // 時間計測の準備
    ch::system_clock::time_point start, end;
    vector<pair<pair<double, double>, double> > precomputation_time_list; // < <範囲開始位置, 範囲終了位置>, 事前計算時間 >
    double bottom = 0;              // 範囲開始位置　
    double top = length_to_divide;  // 範囲終了位置

    using Path_List = vector<vector<int> >;
    using Extended_Sketches = unordered_map<int, vector<Path_List> >;
    Extended_Sketches extended_sketches;
    for (const vector<int>& node_list : divided_list_of_node_list_sorted_by_degree) {


        start = ch::system_clock::now(); // 計測開始


        // sketch 生成
        #pragma omp parallel for
        for (int i = 0; i < node_list.size(); ++i) {
            extended_sketches[node_list[i]] = extended_sketch_index(graph, node_list[i], seed_node_sets, bc_top_node_sets);
        }


        end = ch::system_clock::now();   // 計測終了
        double precomputation_time = static_cast<double>(ch::duration_cast<ch::microseconds>(end - start).count() / 1000.0);
        precomputation_time_list.push_back( { {bottom, top}, precomputation_time } );
        bottom += length_to_divide;
        top += length_to_divide;


    }
    cout << "Complete generating extended sketches" << endl;
    

    /* パスの設定 */
    string result_dir_path =  "./" + graph_name;
    string extended_sketches_path = result_dir_path + "/extended_sketches.txt";


    /* extended_sketches 保存 */
    fs::create_directories(result_dir_path);
    write_extended_sketches(extended_sketches_path, extended_sketches);
    cout << "Complete writing extended sketches" << endl;


    /* 事前計算時間の結果を保存 */
    string precomputation_time_path = result_dir_path + "/precomputation.txt";
    write_precomputation_time(precomputation_time_path, precomputation_time_list);
    cout << "Complete writing precomputation time" << endl;


    return 0;
}

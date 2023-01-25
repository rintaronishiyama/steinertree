#include <vector>
#include <iterator>  // std::back_inserter
#include <algorithm> // std::sample, std::sort
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <filesystem>
#include <utility>   // std::pair
#include <math.h>    // std::ceil
#include <chrono>
#include <cmath>
#include <fstream>
#include "graph.h"
#include "read.h"
#include "sketchLS.h"
#include "write.h"
#include "random.h"
#include "divide.h"
#include "set.h"
#include "x.h"

// Files to compile
// precomputation.cpp graph.cpp read.cpp split.cpp sketchLS.cpp write.cpp random.cpp divide.cpp set.cpp x.cpp

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
using std::to_string;
using std::ceil;

namespace fs = std::filesystem;
namespace ch = std::chrono;

int main(int argc, char* argv[])
{
    /* グラフの選択 */
    string graph_name;
    cout << "Enter graph name : ";
    cin >> graph_name;


    /* 避ける上位ノードの個数の間隔を +1 or +0.1% + ×2 から選択*/
    string sketch_interval;
    cout << "select sketch interval" << endl;
    cout << "1 : increment" << endl;
    cout << "2 : add0.1%" << endl;
    cout << "3 : multiply2" << endl;
    cin >> sketch_interval;
    if (sketch_interval != "1" && sketch_interval != "2" && sketch_interval != "3") {
        cout << "please slect 1 or 2 or 3" << endl;
        return 1;
    }
    if (sketch_interval == "1") {
        sketch_interval = "increment";
    }
    if (sketch_interval == "2") {
        sketch_interval = "add";
    }
    if (sketch_interval == "3") {
        sketch_interval = "multiply";
    }


    /* DC上位 or BC上位 or CC上位のどれを避けた sketch 生成をするか選択 */
    string sketch_mode;
    cout << "select sketch mode (DC or BC or CC) : ";
    cin >> sketch_mode;
    if (sketch_mode != "DC" && sketch_mode != "BC" && sketch_mode != "CC") {
        cout << "please select dc or bc or cc" << endl;
        return 1;
    }


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


    /* シードノード集合の読み込み */
    vector<unordered_set<int> > seed_node_sets;
    string seed_node_sets_path = "./" + graph_name + "/seed_node_sets.txt";
    read_seed_node_sets(seed_node_sets_path, seed_node_sets);
    cout << "Complete reading seed node sets" << endl;


    /* DC or BC or CC の降順にソートしたノードのリストを用意 */
    vector<int> node_list_sorted;
    string centrality_txt_path;

    if (sketch_mode == "DC") {
        centrality_txt_path = graph_name + "/DC.txt";
    }
    if (sketch_mode == "BC") {
        centrality_txt_path = graph_name + "/BC.txt";
    }
    if (sketch_mode == "CC") {
        centrality_txt_path = graph_name + "/CC.txt";
    }

    read_node_list_sorted_by_centrality_from_txt_file(centrality_txt_path, node_list_sorted);
    cout << "Complete sorting node list" << endl;


    /* DC or BC or CC上位ノード集合のリストを取得 */
    // 最大で上位1%を避ける
    int max_size_of_top_node_set = ceil(graph.get_number_of_nodes() * 0.01);
    cout << "max size of top node set : "<< max_size_of_top_node_set << endl;
    vector<unordered_set<int> > top_node_sets;
    if (sketch_interval == "increment") {
        top_node_sets
        = get_node_sets_from_node_list_by_increment(node_list_sorted, max_size_of_top_node_set);
    }
    if (sketch_interval == "add") {
        top_node_sets
        = get_node_sets_from_node_list_by_add(node_list_sorted, max_size_of_top_node_set);
    }
    if (sketch_interval == "multiply") {
        top_node_sets
        = get_node_sets_from_node_list_by_multiply(node_list_sorted, max_size_of_top_node_set);
    }
    
    cout << "Complete getting top node sets" << endl;


    /* x の値のリストを用意 */
    vector<string> x_list = get_x_list_for_top_node_sets(top_node_sets);
    cout << "Complete getting x list" << endl;


    /* 時間計測の準備 */
    // シードノード集合の決定にかかる時間を除く事前計算時間
    vector<double> precomputation_time_ms_list(x_list.size(), 0);


    /* ノード数の準備 */
    int n = graph.get_number_of_nodes();




    /* extended sketches 生成 */
    // 型エイリアス
    using Path_List = vector<vector<int> >;
    using Extended_Sketch = vector<Path_List>;
    using Extended_Sketches = vector<Extended_Sketch>; // index の値がそのままノードid

    Extended_Sketches extended_sketches(n);

    generate_extended_sketches(graph, seed_node_sets, top_node_sets, extended_sketches, precomputation_time_ms_list);

    cout << "Complete generating extended sketches" << endl;



    /* extended sketches の分析 */
    // 避けて経路を生成できた割合(少数のまま)のリスト
    vector<double> avoidability_list = get_avoidability_list(extended_sketches, x_list);


    /* 結果を保存するディレクトリを用意 */
    string result_dir_path =  "./" + graph_name + "/" + sketch_interval + "/" + sketch_mode;
    fs::create_directories(result_dir_path);


    /* extended_sketches 保存 */
    fs::create_directories(result_dir_path);
    string extended_sketches_path = result_dir_path + "/extended_sketches.txt";
    write_extended_sketches(extended_sketches_path, extended_sketches);
    cout << "Complete writing extended sketches" << endl;


    /* 事前計算時間の結果を保存 */
    string precomputation_time_ms_path = result_dir_path + "/precomputation.txt";
    write_precomputation_time(precomputation_time_ms_path, precomputation_time_ms_list, x_list);
    cout << "Complete writing precomputation time" << endl;


    /* xの値のリスト保存 */
    string x_list_path = result_dir_path + "/x_list.txt";
    write_x_list(x_list_path, x_list);
    cout << "Complete writing x list" << endl;


    /* avoidability のリストを保存 */
    string avoidability_list_path = result_dir_path + "/avoidability.txt";
    write_avoidability_list(avoidability_list_path, avoidability_list, x_list);
    cout << "Complete writing avoidability" << endl;


    return 0;
}

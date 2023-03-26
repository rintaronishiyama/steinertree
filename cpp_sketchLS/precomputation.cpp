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
    cout << "4 : allornothing" << endl;
    cin >> sketch_interval;
    if (sketch_interval != "1" && sketch_interval != "2" && sketch_interval != "3" && sketch_interval != "4") {
        cout << "please slect 1 or 2 or 3 or 4" << endl;
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
    if (sketch_interval == "4") {
        sketch_interval = "allornothing";
    }


    // 避ける中心性指標の種類
    vector<string> sketch_centrality_list{"DC", "BC", "CC"};


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


    /* DC, BC, CC の降順にソートしたノードのリストを用意 */
    vector< vector<int> > list_of_node_list_sorted( sketch_centrality_list.size() );
    vector<string> list_of_centrality_txt_path;
    for (const string& sketch_centrality : sketch_centrality_list) {
        string centrality_txt_path = "./" + graph_name + "/" + sketch_centrality + ".txt";
        list_of_centrality_txt_path.push_back(centrality_txt_path);
    }

    for (int i = 0; i < sketch_centrality_list.size(); ++i) {
        cout << "i " << i << endl;
        read_node_list_sorted_by_centrality_from_txt_file(list_of_centrality_txt_path.at(i), list_of_node_list_sorted[i]);
    }
    cout << "Complete sorting node list" << endl;


    /* DC, BC, CC上位ノード集合のリストを取得 */
    // 最大で上位1%を避ける
    int max_size_of_top_node_set = ceil(graph.get_number_of_nodes() * 0.01);
    cout << "max size of top node set : "<< max_size_of_top_node_set << endl;

    // [DCtop_node_sets BCtop_node_sets CCtop_node_sets]
    vector<vector<unordered_set<int> > > list_of_top_node_sets( sketch_centrality_list.size() );

    if (sketch_interval == "increment") {
        for (int i = 0; i < sketch_centrality_list.size(); ++i) {
            list_of_top_node_sets[i] 
                = get_node_sets_from_node_list_by_increment(list_of_node_list_sorted.at(i), max_size_of_top_node_set);
        }
    }
    if (sketch_interval == "add") {
        for (int i = 0; i < sketch_centrality_list.size(); ++i) {
            list_of_top_node_sets[i] 
                = get_node_sets_from_node_list_by_add(list_of_node_list_sorted.at(i), max_size_of_top_node_set);
        }
    }
    if (sketch_interval == "multiply") {
        for (int i = 0; i < sketch_centrality_list.size(); ++i) {
            list_of_top_node_sets[i] 
                = get_node_sets_from_node_list_by_multiply(list_of_node_list_sorted.at(i), max_size_of_top_node_set);
        }
    }
    if (sketch_interval == "allornothing") {
        for (int i = 0; i < sketch_centrality_list.size(); ++i) {
            list_of_top_node_sets[i] 
                = get_node_sets_from_node_list_by_all(list_of_node_list_sorted.at(i), max_size_of_top_node_set);
        }
    }
    
    cout << "Complete getting top node sets" << endl;


    /* x の値のリストを用意 */
    vector<string> x_list = get_x_list_for_top_node_sets(list_of_top_node_sets.at(0));
    cout << "Complete getting x list" << endl;


    /* 時間計測の準備 */
    // シードノード集合の決定にかかる時間を除く事前計算時間
    // DC BC CC の順に格納
    vector<vector<double> > list_of_precomputation_time_ms_list(
        sketch_centrality_list.size(),
        vector<double>(x_list.size(), 0));


    /* ノード数の準備 */
    int n = graph.get_number_of_nodes();




    /* extended sketches 生成 */
    // 型エイリアス
    using Path_List = vector<vector<int> >;
    using Extended_Sketch = vector<Path_List>;
    using Extended_Sketches = vector<Extended_Sketch>; // index の値がそのままノードid

    // DC BC CC
    vector<Extended_Sketches> list_of_extended_sketches;
    
    for (int i = 0; i < sketch_centrality_list.size(); ++i) {
        Extended_Sketches extended_sketches(n);
        list_of_extended_sketches.push_back(extended_sketches);
    }

    for (int i = 0; i < sketch_centrality_list.size(); ++i) {
        generate_extended_sketches(
            graph,
            seed_node_sets,
            list_of_top_node_sets.at(i),
            list_of_extended_sketches[i],
            list_of_precomputation_time_ms_list[i]);
    }


    cout << "Complete generating extended sketches" << endl;



    /* extended sketches の分析 */
    // DC avoidability BC avoidability CC avoidability
    // 避けて経路を生成できた割合(少数のまま)のリスト
    vector<vector<double> > list_of_avoidability_list( sketch_centrality_list.size() );
    for (int i = 0; i < sketch_centrality_list.size(); ++i) {
        list_of_avoidability_list[i] = get_avoidability_list(list_of_extended_sketches.at(i), x_list);    
    }


    /* 結果を保存するディレクトリを用意 */
    // DC BC CC
    vector<string> list_of_result_dir_path;
    for (int i = 0; i < sketch_centrality_list.size(); ++i) {
        string result_dir_path = "./" + graph_name + "/" + sketch_interval + "/" + sketch_centrality_list.at(i);
        fs::create_directories(result_dir_path);
        list_of_result_dir_path.push_back(result_dir_path);
    }


    /* extended_sketches 保存 */
    for (int i = 0; i < sketch_centrality_list.size(); ++i) {
        string extended_sketches_path = list_of_result_dir_path.at(i) + "/extended_sketches.txt";
        write_extended_sketches(extended_sketches_path, list_of_extended_sketches.at(i));
    }
    cout << "Complete writing extended sketches" << endl;


    /* 事前計算時間の結果を保存 */
    for (int i = 0; i < sketch_centrality_list.size(); ++i) {
        string precomputation_time_ms_path = list_of_result_dir_path.at(i) + "/precomputation.txt";
        write_precomputation_time(precomputation_time_ms_path, list_of_precomputation_time_ms_list.at(i), x_list);
    }
    cout << "Complete writing precomputation time" << endl;


    /* xの値のリスト保存 */
    for (int i = 0; i < sketch_centrality_list.size(); ++i) {
        string x_list_path = list_of_result_dir_path.at(i) + "/x_list.txt";
        write_x_list(x_list_path, x_list);
    }
    cout << "Complete writing x list" << endl;


    /* avoidability のリストを保存 */
    for (int i = 0; i < sketch_centrality_list.size(); ++i) {
        string avoidability_list_path = list_of_result_dir_path.at(i) + "/avoidability.txt";
        write_avoidability_list(avoidability_list_path, list_of_avoidability_list.at(i), x_list);
    }
    cout << "Complete writing avoidability" << endl;


    return 0;
}

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

int main(int argc, char* argv[])
{
    /* グラフの選択 */
    string graph_name;
    cout << "Enter graph name : ";
    cin >> graph_name;


    /* 次数上位かBC上位のどちらを避けた sketch 生成をするか選択 */
    string sketch_mode;
    cout << "select sketch mode (degree or bc) : ";
    cin >> sketch_mode;
    if (sketch_mode != "degree" && sketch_mode != "bc") {
        cout << "please select degree or bc" << endl;
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


    /* シードノード集合の決定 */
    vector<unordered_set<int> > seed_node_sets = decide_seed_node_sets(graph);
    cout << "Complete deciding seed node sets" << endl;


    /* 次数 or BC の降順にソートしたノードのリストを用意 */
    vector<int> node_list_sorted;
    if (sketch_mode == "degree") {
        node_list_sorted = graph.get_node_list_sorted_by_degree();
    }
    if (sketch_mode == "bc") {
        string BC_txt_path = graph_name + "/BC.txt";
        read_node_list_sorted_by_bc_from_txt_file(BC_txt_path, node_list_sorted);
    }
    cout << "Complete sorting node list" << endl;


    /* 次数 or BC上位ノード集合のリストを取得 */
    int max_size_of_top_node_set = 256;
    vector<unordered_set<int> > top_node_sets
        = get_node_sets_from_node_list(node_list_sorted, max_size_of_top_node_set); 
    cout << "Complete getting top node sets" << endl;


    /* 次数の降順に sketch 生成 */


    /* 時間計測の準備 */
    // ノードリストを 0.1% 毎に区切る
    double length_to_divide = 0.001;
    vector<vector<int> > divided_list_of_node_list_sorted
        = divide_node_list(node_list_sorted, length_to_divide);
    cout << "Complete dividing node list" << endl;


    // < <範囲開始位置, 範囲終了位置>, <もとの事前計算時間, 提案手法の事前計算時間> >のリストを用意
    vector<pair<pair<double, double>, pair<double, double> > > precomputation_time_pair_list;
    double bottom = 0;              // 範囲開始位置　
    double top = length_to_divide;  // 範囲終了位置

    // 型エイリアス
    using Path_List = vector<vector<int> >;
    using Extended_Sketch = vector<Path_List>;
    using Extended_Sketches = unordered_map<int, Extended_Sketch>;
    
    Extended_Sketches extended_sketches;
    for (const vector<int>& node_list : divided_list_of_node_list_sorted) {
        // 並列処理をしても時間を計測するために用意
        vector<pair<double, double> > tmp_precomputation_time_pair_list(node_list.size());
        
        // sketch 生成
        #pragma omp parallel for
        for (int i = 0; i < node_list.size(); ++i) {
            pair<double, double> tmp_precomputation_time_pair(0, 0);
            extended_sketches[node_list[i]] = extended_sketch_index(graph, node_list[i], seed_node_sets, top_node_sets, tmp_precomputation_time_pair);
            tmp_precomputation_time_pair_list[i] = tmp_precomputation_time_pair;
        }

        // 避けた上位毎に事前計算時間を合計
        pair<double, double> precomputation_time_pair(0, 0);

        for (const pair<double, double>& precomputation_time_pair_ : tmp_precomputation_time_pair_list) {
            precomputation_time_pair.first += precomputation_time_pair_.first;
            precomputation_time_pair.second += precomputation_time_pair_.second;
        }


        precomputation_time_pair_list.push_back( { {bottom, top}, precomputation_time_pair } );
        cout << top * 100 << "% done" << endl;
        bottom += length_to_divide;
        top += length_to_divide;
    }
    cout << "Complete generating extended sketches" << endl;
    

    /* パスの設定 */
    string result_dir_path =  "./" + graph_name + "/" + sketch_mode;
    string extended_sketches_path = result_dir_path + "/extended_sketches.txt";


    /* extended_sketches 保存 */
    fs::create_directories(result_dir_path);
    write_extended_sketches(extended_sketches_path, extended_sketches);
    cout << "Complete writing extended sketches" << endl;


    /* 事前計算時間の結果を保存 */
    string precomputation_time_path = result_dir_path + "/precomputation.txt";
    write_precomputation_time(precomputation_time_path, precomputation_time_pair_list);
    cout << "Complete writing precomputation time" << endl;


    return 0;
}

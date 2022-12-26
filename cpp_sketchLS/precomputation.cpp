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

// Files to compile
// precomputation.cpp graph.cpp read.cpp split.cpp sketchLS.cpp write.cpp random.cpp divide.cpp

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


    /* グラフの情報を表示 */
    cout << "This graph has " << graph.get_number_of_nodes()
        << " nodes and " << graph.get_number_of_edges() << "edges" << endl;


    /* data のパス */
    string result_dir_path =  "./" + graph_name;
    string sketches_path = result_dir_path + "/sketches.txt";


    /* シードノード集合の決定 */
    vector<unordered_set<int> > seed_node_sets = decide_seed_node_sets(graph);
    cout << "Complete deciding seed node sets" << endl;


    /* 次数の降順にソートしたノードのリストを用意 */
    vector<int> node_list_sorted_by_degree = graph.get_node_list_sorted_by_degree();
    cout << "Complete sorting by degree" << endl;
    

    /* ノードリストを 0.1% 毎に区切る */
    double length_to_divide = 0.001;
    vector<vector<int> > divided_list_of_node_list_sorted_by_degree
        = divide_node_list_by_length_to_divide(node_list_sorted_by_degree, length_to_divide);


    /* BC上位ノードを除いた sketch を生成するか確認 */
    cout << "Select sketch generation mode" << endl;
    cout << "1 : normal" << endl;
    cout << "2 : avoid BC top" << endl;
    
    int tmp;
    cin >> tmp;

    string sketch_generation_mode;
    if (tmp == 1) {
        sketch_generation_mode = "normal";
    }
    if (tmp == 2) {
        sketch_generation_mode = "avoid_BC_top";
    }

    // BC上位を除くならBC.txtを読み込み
    string BC_txt_path = "./" + graph_name + "/BC.txt";
    map<int, double> bc_dict; // <node, bc>
    vector<int> nodes_sorted_by_bc;
    if (sketch_generation_mode == "avoid_BC_top") {
        read_bc_from_txt_file(BC_txt_path, bc_dict);
        for (const pair<int, double>& item : bc_dict) {
            nodes_sorted_by_bc.push_back(item.first);
        }
        sort(nodes_sorted_by_bc.begin(),
        nodes_sorted_by_bc.end(),
        [bc_dict](const int& left, const int& right){
            return bc_dict.at(left) > bc_dict.at(right);
        });
    }


    /* BC上位ノードの集合を決定 */
    unordered_set<int> bc_top_nodes;
    double avoid_bc_top_rate;
    if (sketch_generation_mode == "avoid_BC_top") {
        cout << "avoid bc top rate (e.g., 0.2):";
        cin >> avoid_bc_top_rate;
        int number_of_avoid_bc_top = ceil(avoid_bc_top_rate * bc_dict.size());
        int count = 0;
        for (const int& node : nodes_sorted_by_bc) {
            if (count >= number_of_avoid_bc_top) {
                break;
            }
            bc_top_nodes.insert(node);
            cout << node << " : " << bc_dict[node] << endl;
            ++count;
        }
    }


    /* 次数の降順に sketch 生成 */
    // 時間計測の準備
    ch::system_clock::time_point start, end;
    vector<pair<pair<double, double>, double> > precomputation_time_list; // < <範囲開始位置, 範囲終了位置>, 事前計算時間 >
    double bottom = 0;          // 範囲開始位置　
    double top = length_to_divide;  // 範囲終了位置

    unordered_map<int, vector <vector<int> > > sketches;
    for (const vector<int>& node_list_divided : divided_list_of_node_list_sorted_by_degree) {
        start = ch::system_clock::now();

        // sketch 生成
        #pragma omp parallel for
        for (int i = 0; i < node_list_divided.size(); ++i) {
            if (sketch_generation_mode == "normal") {
                sketches[node_list_divided[i]] = sketch_index(graph, node_list_divided[i], seed_node_sets);
            }
            if (sketch_generation_mode == "avoid_BC_top") {
                sketches[node_list_divided[i]] = sketch_index_avoiding_bc_top(graph, node_list_divided[i], seed_node_sets, bc_top_nodes);
            }
        }

        end = ch::system_clock::now();
        double precomputation_time = static_cast<double>(ch::duration_cast<ch::microseconds>(end - start).count() / 1000.0);
        precomputation_time_list.push_back( { {bottom, top}, precomputation_time } );
        bottom += length_to_divide;
        top += length_to_divide;
    }
    
    /* sketches 保存 */
    fs::create_directories(result_dir_path);
    if (sketch_generation_mode == "normal") {
        write_sketches(sketches_path, sketches);
    } else if (sketch_generation_mode == "avoid_BC_top") {
        sketches_path = result_dir_path + "/sketches_avoid" + std::to_string(avoid_bc_top_rate) + "bc.txt";
        write_sketches(sketches_path, sketches);
    }


    /* 事前計算時間の結果を保存 */
    if (sketch_generation_mode == "normal") {
        string precomputation_time_path = result_dir_path + "/precomputation.txt";
        write_precomputation_time(precomputation_time_path, precomputation_time_list);
    } else if (sketch_generation_mode == "avoid_BC_top") {
        string precomputation_time_path = result_dir_path + "/precomputation_avoid" + std::to_string(avoid_bc_top_rate) + "bc.txt";
        write_precomputation_time(precomputation_time_path, precomputation_time_list);
    }


    return 0;
}

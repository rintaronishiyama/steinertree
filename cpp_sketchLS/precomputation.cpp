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
#include <fstream>
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
using std::to_string;

namespace fs = std::filesystem;
namespace ch = std::chrono;

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


    /* 時間計測の準備 */
    ch::system_clock::time_point start, end;
    vector<pair<string, double> > precomputation_time_list;
    double elapsed;


    /* sketches 生成 */
    // node_list の準備
    vector<int> node_list = graph.get_node_list();

    // n の準備
    int n = graph.get_number_of_nodes();

    // 型エイリアス
    using Sketch = vector<vector<int> >;
    using Sketches = vector<Sketch>;    // index の値がそのままノードid

    // ver1
    Sketches sketches1(n);
    
    start = ch::system_clock::now();
    #pragma omp parallel for
    for (int i = 0; i < node_list.size(); ++i) {
        sketches1[node_list.at(i)] = sketch_index(graph, node_list.at(i), seed_node_sets);
    }
    end   = ch::system_clock::now();
    
    cout << "sketch ver 1 end" << endl;
    elapsed = static_cast<double>(ch::duration_cast<ch::milliseconds>(end - start).count());
    precomputation_time_list.push_back({"sketch ver1", elapsed});


    // ver2
    Sketches sketches2(n);

    start = ch::system_clock::now();
    generate_sketches(graph, seed_node_sets, sketches2);
    end   = ch::system_clock::now();
    
    cout << "sketch ver 2 end" << endl;
    elapsed = static_cast<double>(ch::duration_cast<ch::milliseconds>(end - start).count());
    precomputation_time_list.push_back({"sketch ver2", elapsed});


    /* extended sketches 生成 */
    // 型エイリアス
    using Path_List = vector<vector<int> >;
    using Extended_Sketch = vector<Path_List>;
    using Extended_Sketches = vector<Extended_Sketch>; // index の値がそのままノードid

    // ver1
    Extended_Sketches extended_sketches1(n);
    
    start = ch::system_clock::now();
    #pragma omp parallel for
    for (int i = 0; i < node_list.size(); ++i) {
        extended_sketches1[node_list.at(i)] = extended_sketch_index(graph, node_list.at(i), seed_node_sets, top_node_sets);
    }
    end   = ch::system_clock::now();

    cout << "extended sketch ver 1 end" << endl;
    elapsed = static_cast<double>(ch::duration_cast<ch::milliseconds>(end - start).count());
    precomputation_time_list.push_back({"extended sketch ver1", elapsed});


    // ver2
    Extended_Sketches extended_sketches2(n);

    start = ch::system_clock::now();
    generate_extended_sketches(graph, seed_node_sets, top_node_sets, extended_sketches2);
    end   = ch::system_clock::now();

    cout << "extended sketch ver 2 end" << endl;
    elapsed = static_cast<double>(ch::duration_cast<ch::milliseconds>(end - start).count());
    precomputation_time_list.push_back({"extended sketch ver2", elapsed});

    /* 結果を保存するディレクトリを用意 */
    string result_dir_path =  "./" + graph_name + "/" + sketch_mode;
    fs::create_directories(result_dir_path);


    /* extended_sketches 保存 */
    fs::create_directories(result_dir_path);
    string extended_sketches_path = result_dir_path + "/extended_sketches.txt";
    write_extended_sketches(extended_sketches_path, extended_sketches2);
    cout << "Complete writing extended sketches" << endl;


    /* 事前計算時間の結果を保存 */
    string precomputation_time_path = result_dir_path + "/precomputation.txt";
    write_precomputation_time(precomputation_time_path, precomputation_time_list);
    cout << "Complete writing precomputation time" << endl;


    return 0;
}

#include <string>
#include <iostream>
#include <filesystem>
#include <unordered_set>
#include <vector>
#include <chrono>
#include "read.h"
#include "write.h"
#include "graph.h"
#include "random.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::unordered_set;
using std::vector;

namespace fs = std::filesystem;
namespace ch = std::chrono;

// Files to compile
// seed.cpp read.cpp write.cpp graph.cpp random.cpp split.cpp


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
    ch::system_clock::time_point start = ch::system_clock::now();
    vector<unordered_set<int> > seed_node_sets = decide_seed_node_sets(graph);
    ch::system_clock::time_point end   = ch::system_clock::now();

    double elapsed = static_cast<double>(ch::duration_cast<ch::milliseconds>(end - start).count());
    cout << "Complete deciding seed node sets" << endl;


    /* シードノード集合を保存 */
    fs::create_directories("./" + graph_name);
    string seed_node_sets_path = "./" + graph_name + "/seed_node_sets.txt";
    write_seed_node_sets(seed_node_sets_path, seed_node_sets);

    /* 計算時間を保存 */
    string seed_node_sets_time_path = "./" + graph_name + "/seed_node_sets_time.txt";
    write_seed_node_sets_time(seed_node_sets_time_path, elapsed);

    return 0;
}


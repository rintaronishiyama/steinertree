#include <vector>
#include <iostream>
#include <string>
#include <filesystem>
#include "random.h"
#include "write.h"
#include "graph.h"
#include "read.h"

// Files to compile
// query.cpp random.cpp write.cpp graph.cpp read.cpp split.cpp

using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::string;

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
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


    /* ターミナル数の決定 */    
    int number_of_terminals;
    cout << "number of terminals : ";
    cin >> number_of_terminals;


    /* 試行回数の決定 */
    int number_of_trials;
    cout << "number of trials : ";
    cin >> number_of_trials;


    /* terminals のリストを作成 */
    vector<vector<int> > list_of_terminals;
    for (int i = 0; i < number_of_trials; ++i) {
        vector<int> terminals = decide_terminals(graph, number_of_terminals);
        list_of_terminals.push_back(terminals);
    }


    /* terminals のリストを書き込み */
    string terminals_path = graph_name + "/terminals.txt";
    write_terminals(terminals_path, list_of_terminals);

    return 0;
}
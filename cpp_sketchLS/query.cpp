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
using std::to_string;

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
    int min_num_of_terminals = 3;
    int max_num_of_terminals = 7;


    /* 試行回数の決定 */
    int number_of_trials = 1000;


    /* terminals のリストを作成 */
    vector<vector<vector<int> > > list_of_list_of_terminals;
    for (int i = min_num_of_terminals; i <= max_num_of_terminals; ++i) {
        vector<vector<int> > list_of_terminals;

        for (int j = 0; j < number_of_trials; ++j) {
        vector<int> terminals = decide_terminals(graph, i);
        list_of_terminals.push_back(terminals);
        }

        list_of_list_of_terminals.push_back(list_of_terminals);
    }


    /* terminals のリストを書き込み */
    vector<string> terminals_path_list;
    for (int i = min_num_of_terminals; i <= max_num_of_terminals; ++i) {
        string terminals_path = graph_name + "/terminals" + to_string(i) + ".txt";
        terminals_path_list.push_back(terminals_path);
    }

    for (int i = 0; i < list_of_list_of_terminals.size(); ++i) {
        write_list_of_terminals(terminals_path_list[i], list_of_list_of_terminals[i]);
    }

    return 0;
}
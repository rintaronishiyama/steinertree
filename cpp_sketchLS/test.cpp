#include <vector>
#include <iostream>
#include <string>
#include "graph.h"
#include "read.h"

using namespace std;


int main(int argc, char* argv[])
{
    /* グラフ読み込み */
    Graph graph;
    string dataset_path = "../dataset/test_dataset.txt";
    read_graph_from_txt_file(dataset_path, graph);
    
    Graph copy{graph};
    copy.add_edge(0, 15);
    cout << "original" << endl;
    for (int node : graph.get_adjacency_list().at(0)) {
        cout << node << endl;
    }
    cout << "copy" << endl;
    for (int node : copy.get_adjacency_list().at(0)) {
        cout << node << endl;
    }
}
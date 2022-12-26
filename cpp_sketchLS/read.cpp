#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include "graph.h"
#include "split.h"

using std::vector;
using std::ifstream;
using std::string;
using std::cout;
using std::endl;
using std::stoi;
using std::unordered_map;
using std::map;

void read_graph_from_txt_file(string file_path, Graph& graph) {
    ifstream ifs(file_path);
    if (!ifs) {
        throw "Failed to open file";
    }

    string line;

    while ( getline(ifs, line) ) {
        vector<string> str_list = split(line, ' ');
        int n1 = stoi(str_list[0]);
        int n2 = stoi(str_list[1]);
        graph.add_edge(n1, n2);
    }
}



void read_sketches_from_txt_file(
    string file_path,
    unordered_map<int, vector<vector<int> > >& sketches)
{
    ifstream ifs(file_path);
    if (!ifs) {
        throw "Failed to open file";
    }

    string line;

    while( getline(ifs, line) ) {
        vector<string> str_list = split(line, ' ');
        int sketch_node = stoi( str_list.front() );
        str_list.erase(str_list.begin(), str_list.begin() + 2); // sketch_node と : を取り除く

        vector<vector<int> > tmp_sketch;
        vector<int> tmp_shortest_path_for_seed_set;
        
        for (string str : str_list) {
            if (str == "end") {
                break;
            }
            if (str == "next") {
                tmp_sketch.push_back(tmp_shortest_path_for_seed_set); // next までの最短経路を追加
                tmp_shortest_path_for_seed_set.clear();               // 全要素削除
                continue;
            }
            tmp_shortest_path_for_seed_set.push_back( stoi(str) );
        }
        sketches[sketch_node] = tmp_sketch;
    }
}

void read_bc_from_txt_file(
    string file_path,
    map<int, double>& bc_dict)
{
    ifstream ifs(file_path);
    if (!ifs) {
        throw "Failed to open file";
    }

    string line;

    while( getline(ifs, line) ) {
        vector<string> str_list = split(line, ' ');
        int node = stoi(str_list[0]);
        double bc = stod(str_list[1]);

        bc_dict[node] = bc;
    }
}
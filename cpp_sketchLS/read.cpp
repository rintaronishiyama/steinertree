#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
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
using std::unordered_set;
using std::map;

// 指定したファイルからグラフを読み込み
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


// 指定したファイルから sketches を読み込み
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
        
        for (const string& str : str_list) {
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


// 指定したファイルから extended sketches を読み込み
void read_extended_sketches_from_txt_file(
    string file_path,
    unordered_map<int, vector<vector<vector<int> > > >& extended_sketches)
{
    ifstream ifs(file_path);
    if (!ifs) {
        throw "Failed to open file";
    }

    string line;

    while ( getline(ifs, line) ) {
        vector<string> str_list = split(line, ' ');
        int sketch_node = stoi( str_list.front() );
        str_list.erase(str_list.begin(), str_list.begin() + 2); // sketch_node と : を取り除く

        vector<vector<vector<int> > > tmp_extended_sketch;
        vector<vector<int> > tmp_path_list_for_seed_set;
        vector<int> tmp_path;

        for (const string& str : str_list) {
            if (str == "end") {
                break;
            }
            if (str == "L") {
                tmp_extended_sketch.push_back(tmp_path_list_for_seed_set); // 経路リストを追加
                tmp_path_list_for_seed_set.clear();                        // 経路リストをリセット
                continue;
            }
            if (str == "P") {
                tmp_path_list_for_seed_set.push_back(tmp_path); // 経路を追加
                tmp_path.clear();                               // 経路をリセット
                continue;
            }

            tmp_path.push_back( stoi(str) );
        }

        extended_sketches[sketch_node] = tmp_extended_sketch;
    }
}


void read_node_list_sorted_by_centrality_from_txt_file(
    string file_path,
    vector<int>& node_list_sorted_by_centrality)
{
    ifstream ifs(file_path);
    if (!ifs) {
        throw "Failed to open file";
    }

    string line;

    while( getline(ifs, line) ) {
        vector<string> str_list = split(line, ' ');
        int node = stoi(str_list[0]);
        node_list_sorted_by_centrality.push_back(node);
    }
}


void read_list_of_terminals_from_txt_file(
    string file_path,
    vector<vector<int> >& list_of_terminals)
{
    ifstream ifs(file_path);
    if (!ifs) {
        throw "Failed to open file";
    }

    string line;

    while( getline(ifs, line) ) {
        vector<int> tmp_terminals;

        vector<string> str_list = split(line, ' ');
        for (const string& str : str_list) {
            tmp_terminals.push_back( stoi(str) );
        }

        list_of_terminals.push_back(tmp_terminals);
    }
}


void read_centrality_from_txt_file(
    string file_path,
    unordered_map<int, double>& centrality_map)
{
    ifstream ifs(file_path);
    if (!ifs) {
        throw "Failed to open file";
    }

    string line;

    while( getline(ifs, line) ) {
        vector<string> str_list = split(line, ' ');
        
        int node = stoi( str_list.at(0) );
        double bc = stod( str_list.at(1) );

        centrality_map[node] = bc;
    }
}


void read_seed_node_sets(
    string file_path,
    vector<unordered_set<int> >& seed_node_sets)
{
    ifstream ifs(file_path);
    if (!ifs) {
        throw "Failed to open file";
    }

    string line;

    while( getline(ifs, line) ) {
        vector<string> str_list = split(line, ' ');
        
        unordered_set<int> seed_node_set;

        for (const string& str : str_list) {
            seed_node_set.insert( stoi(str) );
        }

        seed_node_sets.push_back(seed_node_set);
    }
}


void read_x_list(
    string file_path,
    vector<string>& x_list)
{
    ifstream ifs(file_path);
    if (!ifs) {
        throw "Failed to open file";
    }

    string line;

    while( getline(ifs, line) ) {
        vector<string> str_list = split(line, ' ');
        
        x_list.push_back( str_list.at(0) );
    }
}
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <utility> // std::pair
#include <fstream>
#include "graph.h"

using std::string;
using std::vector;
using std::unordered_map;
using std::unordered_set;
using std::ofstream;
using std::endl;
using std::pair;

void write_sketches(
    string file_path,
    const vector<vector<vector<int> > >& sketches)
{
    ofstream ofs(file_path);

    for (int i = 0; i < sketches.size(); ++i) {
        ofs << i << " : ";
        for (const vector<int>& shortest_path_for_seed_set : sketches.at(i)) {
            for (int node : shortest_path_for_seed_set) {
                ofs << node << " ";
            }
            
            ofs << "next ";
        }
        
        ofs << "end" << endl;
    }
}

void write_extended_sketches(
    string file_path,
    const vector<vector<vector<vector<int> > > >& extended_sketches)
{
    ofstream ofs(file_path);

    for (int i = 0; i < extended_sketches.size(); ++i) {
        ofs << i << " : ";
        for (const vector<vector<int> >& path_list_for_seed_set : extended_sketches.at(i)) {
            for (const vector<int>& path : path_list_for_seed_set) {
                for (const int& node : path) {
                    ofs << node << " ";
                }

                ofs << "P ";    // パスの末尾の意
            }

            ofs << "L ";        // リストの末尾の意
        }

        ofs << "end" << endl;   // 行末の意
    }
}

void write_list_of_terminals(
    string file_path,
    const vector<vector<int> >& list_of_terminals)
{
    ofstream ofs(file_path);

    for (const vector<int>& terminals : list_of_terminals) {
        for (const int& terminal : terminals) {
            if (terminal == terminals.back()) {
                ofs << terminal << endl;
                break;
            }
            ofs << terminal << " ";
        }
    }
}

void write_graph(string file_path, const Graph& graph) {
    ofstream ofs(file_path);

    for (const pair<int, vector<int> >& item : graph.get_adjacency_list() ) {
        ofs << item.first << " : ";
        for (int node : item.second) {
            ofs << node << " ";
        }
        ofs << endl;
    }
}

void write_terminals_to_exisiting_txt(string file_path, const vector<int>& terminals){
    ofstream ofs(file_path, std::ios::app); // 追加書き込みのため第2引数を指定
    
    for (const int& terminal : terminals) {
        ofs << terminal << " ";
    }

    ofs << endl;
}

void write_precomputation_time(
    string file_path,
    const vector<double>& precomputation_time_ms_list,
    const vector<string>& x_list)
{
    ofstream ofs(file_path);

    for (int i = 0; i < x_list.size(); ++i) {
        ofs << x_list.at(i) << " " << precomputation_time_ms_list.at(i) << endl;
    }
}


void write_evaluation(
    string file_path,
    const vector<string>& x_list,
    const vector<double>& list_of_evaluation)
{
    ofstream ofs(file_path);

    for (int i = 0; i < x_list.size(); ++i) {
        ofs << x_list.at(i) << " " << list_of_evaluation.at(i) << endl;
    }
}


void write_seed_node_sets (
    string file_path,
    const vector<unordered_set<int> >& seed_node_sets)
{
    ofstream ofs(file_path);

    for (const unordered_set<int>& seed_node_set : seed_node_sets) {
        for (const int& seed_node : seed_node_set) {
            ofs << seed_node << " ";
        }

        ofs << endl;
    }
}

void write_seed_node_sets_time(
    string file_path,
    const double& elapsed)
{
    ofstream ofs(file_path);

    ofs << elapsed << endl;
}

void write_x_list (
    string file_path,
    const vector<string>& x_list)
{
    ofstream ofs(file_path);

    for (const string& x : x_list) {
        ofs << x << endl;
    }
}


void write_avoidability_list (
    string file_path,
    const vector<double>& avoidability_list,
    const vector<string>& x_list)
{
    ofstream ofs(file_path);

    for (int i = 0; i < avoidability_list.size(); ++i) {
        ofs << x_list.at(i) << " " << avoidability_list.at(i) << endl;
    }
}
#include <string>
#include <vector>
#include <unordered_map>
#include <utility> // std::pair
#include <fstream>
#include "graph.h"

using std::string;
using std::vector;
using std::unordered_map;
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
    const vector<pair<string, double> >& precomputation_time_pair_list)
{
    ofstream ofs(file_path);

    for (const pair<string, double>& item : precomputation_time_pair_list) {
        ofs << item.first << " : " << item.second << "ms" << endl;
    }
}

void write_overlap_ratio(
    string file_path,
    const vector<string>& x_list_for_avoided_bc_top_nodes, 
    const vector<string>& x_list_for_limit_range,
    const vector<vector<double> >& list_of_list_of_overlap_raio)
{
    ofstream ofs(file_path);

    for (int i = 0; i < list_of_list_of_overlap_raio.size(); ++i) {
        for (int j = 0; j < list_of_list_of_overlap_raio.at(i).size(); ++j) {
            ofs << x_list_for_avoided_bc_top_nodes.at(i) << " "
                << x_list_for_limit_range.at(j) << " "
                << list_of_list_of_overlap_raio.at(i).at(j) << endl;
        }
    }
}

void write_ST_size(
    string file_path,
    const vector<string>& x_list_for_avoided_bc_top_nodes, 
    const vector<string>& x_list_for_limit_range,
    const vector<vector<double> >& list_of_list_of_ST_size)
{
    ofstream ofs(file_path);

    for (int i = 0; i < list_of_list_of_ST_size.size(); ++i) {
        for (int j = 0; j < list_of_list_of_ST_size.at(i).size(); ++j) {
            ofs << x_list_for_avoided_bc_top_nodes.at(i) << " "
                << x_list_for_limit_range.at(j) << " "
                << list_of_list_of_ST_size.at(i).at(j) << endl;
        }
    }
}


void write_sum_of_degree(
    string file_path,
    const vector<string>& x_list_for_avoided_bc_top_nodes, 
    const vector<string>& x_list_for_limit_range,
    const vector<vector<double> >& list_of_list_of_sum_of_degree)
{
    ofstream ofs(file_path);

    for (int i = 0; i < list_of_list_of_sum_of_degree.size(); ++i) {
        for (int j = 0; j < list_of_list_of_sum_of_degree.at(i).size(); ++j) {
            ofs << x_list_for_avoided_bc_top_nodes.at(i) << " "
                << x_list_for_limit_range.at(j) << " "
                << list_of_list_of_sum_of_degree.at(i).at(j) << endl;
        }
    }
}


void write_sum_of_bc(
    string file_path,
    const vector<string>& x_list_for_avoided_bc_top_nodes, 
    const vector<string>& x_list_for_limit_range,
    const vector<vector<double> >& list_of_list_of_sum_of_bc)
{
    ofstream ofs(file_path);

    for (int i = 0; i < list_of_list_of_sum_of_bc.size(); ++i) {
        for (int j = 0; j < list_of_list_of_sum_of_bc.at(i).size(); ++j) {
            ofs << x_list_for_avoided_bc_top_nodes.at(i) << " "
                << x_list_for_limit_range.at(j) << " "
                << list_of_list_of_sum_of_bc.at(i).at(j) << endl;
        }
    }
}

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
    const unordered_map<int, vector<vector<int> > >& sketches)
{
    ofstream ofs(file_path);
    for (const pair<int, vector<vector<int> > >& item : sketches) {
        ofs << item.first << " : ";
        for (vector<int> shortest_path_for_seed_set : item.second) {
            for (int node : shortest_path_for_seed_set) {
                ofs << node << " ";
            }
            
            ofs << "next ";
        }
        
        ofs << "end" << endl;
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

void write_precomputation_time(
    std::string file_path,
    const std::vector<std::pair<std::pair<double, double>, double> >& precomputation_time_list)
{
    ofstream ofs(file_path);
    for (const pair<pair<double, double>, double>& item : precomputation_time_list) {
        double bottom_percentage = item.first.first * 100;
        double top_percentage = item.first.second * 100;
        ofs << bottom_percentage << "% ~ " << top_percentage << "% : " << item.second << "ms" << endl;
    }
}


void write_overlap_ratio(
    string file_path,
    const vector<pair<double, double> >& sketches_range_list,
    const vector<double>& overlap_ratio_list
) 
{
    ofstream ofs(file_path);

    for (int i = 0; i < sketches_range_list.size(); ++i) {
        pair range = sketches_range_list.at(i);
        double overlap_ratio = overlap_ratio_list.at(i);
        ofs << range.first << " " << range.second;
        ofs << " : " << overlap_ratio << endl;
    }
}
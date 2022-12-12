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
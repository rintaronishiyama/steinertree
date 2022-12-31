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

void write_extended_sketches(
    string file_path,
    const unordered_map<int, vector<vector<vector<int> > > >& extended_sketches)
{
    ofstream ofs(file_path);
    for (const pair<int, vector<vector<vector<int> > > >& item : extended_sketches) {
        ofs << item.first << " : ";
        for (const vector<vector<int> >& path_list_for_seed_set : item.second) {
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

void write_terminals(
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
    const vector<double>& overlap_ratio_list) 
{
    ofstream ofs(file_path);

    for (int i = 0; i < sketches_range_list.size(); ++i) {
        pair range = sketches_range_list.at(i);
        double overlap_ratio = overlap_ratio_list.at(i);
        ofs << range.first << " " << range.second;
        ofs << " : " << overlap_ratio << endl;
    }
}

void write_ST_size(
    string file_path,
    const vector<pair<double, double> >& sketches_range_list,
    const vector<double>& ST_size_list)
{
    ofstream ofs(file_path);

    ofs << "original : " << ST_size_list.at(0) << endl;
    for (int i = 1; i < sketches_range_list.size(); ++i) {
        pair range = sketches_range_list.at(i);
        double size = ST_size_list.at(i + 1);
        ofs << range.first << " " << range.second;
        ofs << " : " << size << endl;
    }
}

void write_overlap_ratio(
    string file_path,
    const vector<string>& x_list_for_list_of_sketches,
    const vector<double>& overlap_ratio_list)   
{
    ofstream ofs(file_path);

    for (int i = 0; i < overlap_ratio_list.size(); ++i) {
        ofs << x_list_for_list_of_sketches.at(i + 1) << " "
            << overlap_ratio_list.at(i) << endl;
    }
}

void write_ST_size(
    string file_path,
    const vector<string>& x_list_for_list_of_sketches,
    const vector<double>& ST_size_list)    
{   
    ofstream ofs(file_path);

    for (int i = 0; i < ST_size_list.size(); ++i) {
        ofs << x_list_for_list_of_sketches.at(i) << " "
            << ST_size_list.at(i) << endl;
    }
}
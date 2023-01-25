#ifndef GUARD_READ_H
#define GUARD_READ_H

#include <string>
#include <unordered_map>
#include <unordered_set>

class Graph;

void read_graph_from_txt_file(std::string file_path, Graph& graph);

void read_sketches_from_txt_file(
    std::string file_path,
    std::unordered_map<int, std::vector<std::vector<int> > >& sketches);

void read_extended_sketches_from_txt_file(
    std::string file_path,
    std::unordered_map<int, std::vector<std::vector<std::vector<int> > > >& extended_sketches);

void read_node_list_sorted_by_centrality_from_txt_file(
    std::string file_path,
    std::vector<int>& node_list_sorted_by_centrality);

void read_list_of_terminals_from_txt_file(
    std::string file_path,
    std::vector<std::vector<int> >& list_of_terminals);

void read_centrality_from_txt_file(
    std::string file_path,
    std::unordered_map<int, double>& centrality_map);

void read_seed_node_sets(
    std::string file_path,
    std::vector<std::unordered_set<int> >& seed_node_sets
);

void read_x_list(
    std::string file_path,
    std::vector<std::string>& x_list
);

#endif // GUARD_READ_H
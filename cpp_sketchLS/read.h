#ifndef GUARD_READ_H
#define GUARD_READ_H

#include <string>
#include <unordered_map>
#include <map>

class Graph;

void read_graph_from_txt_file(std::string file_path, Graph& graph);

void read_sketches_from_txt_file(
    std::string file_path,
    std::unordered_map<int, std::vector<std::vector<int> > >& sketches);

void read_extended_sketches_from_txt_file(
    std::string file_path,
    std::unordered_map<int, std::vector<std::vector<std::vector<int> > > >& extended_sketches);

void read_bc_from_txt_file(
    std::string file_path,
    std::vector<int>& node_list_sorted_by_bc
);


#endif // GUARD_READ_H
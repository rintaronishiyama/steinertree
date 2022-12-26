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

void read_bc_from_txt_file(
    std::string file_path,
    std::map<int, double>& bc_dict
);

#endif // GUARD_READ_H
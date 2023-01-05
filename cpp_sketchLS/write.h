#ifndef GUARD_WRITE_H
#define GUARD_WRITE_H

#include <vector>
#include <unordered_map>
#include <string>
#include <utility>

class Graph;

void write_sketches(
    std::string file_path,
    const std::unordered_map<int, std::vector<std::vector<int> > >& sketches);

void write_extended_sketches(
    std::string file_path,
    const std::unordered_map<int, std::vector<std::vector<std::vector<int> > > >& extended_sketches);

void write_list_of_terminals(
    std::string file_path,
    const std::vector<std::vector<int> >& list_of_terminals);

void write_graph(std::string file_path, const Graph& graph);

void write_terminals_to_exisiting_txt(std::string file_path, const std::vector<int>& terminals);

void write_precomputation_time(
    std::string file_path,
    const std::vector<std::pair<std::pair<double, double>, double> >& precomputation_time_list);

void write_overlap_ratio(
    std::string file_path,
    const std::vector<std::string>& x_list_for_avoided_bc_top_nodes,
    const std::vector<std::string>& x_list_for_limit_range,
    const std::vector<std::vector<double> >& list_of_list_of_overlap_ratio
);

void write_ST_size(
    std::string file_path,
    const std::vector<std::string>& x_list_for_avoided_bc_top_nodes,
    const std::vector<std::string>& x_list_for_limit_range,
    const std::vector<std::vector<double> >& list_of_list_of_ST_size
);

#endif // GUARD_WRITE_H
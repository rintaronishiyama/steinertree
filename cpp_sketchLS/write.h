#ifndef GUARD_WRITE_H
#define GUARD_WRITE_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <utility>

class Graph;

void write_sketches(
    std::string file_path,
    const std::vector<std::vector<std::vector<int> > >& sketches);

void write_extended_sketches(
    std::string file_path,
    const std::vector<std::vector<std::vector<std::vector<int> > > >& extended_sketches);

void write_list_of_terminals(
    std::string file_path,
    const std::vector<std::vector<int> >& list_of_terminals);

void write_graph(std::string file_path, const Graph& graph);

void write_terminals_to_exisiting_txt(std::string file_path, const std::vector<int>& terminals);

void write_precomputation_time(
    std::string file_path,
    const std::vector<double>& precomputation_time_ms_list,
    const std::vector<std::string>& x_list);

void write_evaluation(
    std::string file_path,
    const std::vector<std::string>& x_list,
    const std::vector<double>& list_of_evaluation
);

void write_seed_node_sets(
    std::string file_path,
    const std::vector<std::unordered_set<int> >& seed_node_sets
);

void write_seed_node_sets_time(
    std::string file_path,
    const double& elapsed
);

void write_x_list(
    std::string file_path,
    const std::vector<std::string>& x_list
);

void write_avoidability_list(
    std::string file_path,
    const std::vector<double>& avoidability_list,
    const std::vector<std::string>& x_list
);

#endif // GUARD_WRITE_H
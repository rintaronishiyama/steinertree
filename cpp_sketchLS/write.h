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

void write_graph(std::string file_path, const Graph& graph);

void write_precomputation_time(
    std::string file_path,
    const std::vector<std::pair<std::pair<double, double>, double> >& precomputation_time_list);

void write_overlap_ratio(
    std::string file_path,
    const std::vector<std::pair<double, double> >& sketches_range_list,
    const std::vector<double>& overlap_ratio_list
);


#endif // GUARD_WRITE_H
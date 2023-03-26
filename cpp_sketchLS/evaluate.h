#ifndef GUARD_EVALUATE_H
#define GUARD_EVALUATE_H

#include <vector>
#include <unordered_map>

class Graph;

double evaluate_overlap_ratio(
    const Graph& ST_for_sketchLS,
    const Graph& ST_for_partial_sketchLS,
    const std::vector<int>& terminals
);

double evaluate_sum_of_degree(
    const Graph& graph,
    const Graph& ST,
    const std::vector<int>& terminals
);

double evaluate_sum_of_centrality(
    const std::unordered_map<int, double>& centrality_map,
    const Graph& ST,
    const std::vector<int>& terminals
);

// 下位関数
std::vector<int> get_node_list_without_terminals(
    const std::vector<int>& node_list,
    const std::vector<int>& terminals
);

#endif // GUARD_EVALUATE_H
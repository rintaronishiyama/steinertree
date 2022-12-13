#ifndef GUARD_RANDOM_H
#define GUARD_RANDOM_H

#include <vector>
#include <unordered_set>

class Graph;

int get_random_element(const std::vector<int>& vec);

std::vector<int> decide_terminals(const Graph& graph, int size);

std::vector<std::unordered_set<int> > decide_seed_node_sets(const Graph& graph);

#endif // GUARD_RANDOM_H
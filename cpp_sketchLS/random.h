#ifndef GUARD_RANDOM_H
#define GUARD_RANDOM_H

#include <vector>

class Graph;

int get_random_element(const std::vector<int>& vec);

std::vector<int> decide_terminals(const Graph& graph, int size);

#endif // GUARD_RANDOM_H
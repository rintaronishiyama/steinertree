#ifndef GUARD_X_H
#define GUARD_X_H

#include <vector>
#include <string>
#include <unordered_set>

std::vector<std::string> get_x_list_for_top_node_sets(
    const std::vector<std::unordered_set<int> >& top_node_sets);

#endif // GUARD_X_H
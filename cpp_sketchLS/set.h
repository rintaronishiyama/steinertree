#ifndef GUARD_SET_H
#define GUARD_SET_H

#include <vector>
#include <unordered_set>

std::vector<std::unordered_set<int> > get_node_sets_from_node_list_by_increment(
    const std::vector<int>& node_list,
    int max_size
);

std::vector<std::unordered_set<int> > get_node_sets_from_node_list_by_add(
    const std::vector<int>& node_list,
    int max_size
);

std::vector<std::unordered_set<int> > get_node_sets_from_node_list_by_multiply(
    const std::vector<int>& node_list,
    int max_size
);

#endif // GUARD_SET_H
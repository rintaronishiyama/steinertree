#ifndef GUARD_SET_H
#define GUARD_SET_H

#include <vector>
#include <unordered_set>

std::vector<std::unordered_set<int> > get_node_sets_from_node_list(
    const std::vector<int>& node_list,
    int max_size
);

#endif // GUARD_SET_H
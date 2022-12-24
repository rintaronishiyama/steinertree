#ifndef GUARD_DEVIDE_H
#define GUARD_DEVIDE_H

#include <vector>
#include <unordered_map>

std::vector<std::vector<int> > divide_node_list_by_length_to_divide(
    const std::vector<int>& node_list,
    double length_to_divide);

std::vector<std::unordered_map<int, std::vector<std::vector<int> > > > divide_sketches_by_length_to_divide(
    const std::unordered_map<int, std::vector<std::vector<int> > >& sketches,
    double length_to_divide,
    const std::vector<int>& node_list_sorted_by_degree
);

#endif // GUARD_DEVIDE_H
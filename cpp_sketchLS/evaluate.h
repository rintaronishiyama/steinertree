#ifndef GUARD_EVALUATE_H
#define GUARD_EVALUATE_H

#include <vector>

class Graph;

double evaluate_overlap_ratio(
    const Graph& ST_for_sketchLS,
    const Graph& ST_for_partial_sketchLS,
    const std::vector<int>& terminals
);

#endif // GUARD_EVALUATE_H
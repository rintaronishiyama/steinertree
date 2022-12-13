#ifndef GUARD_SKETCHLS_H
#define GUARD_SKETCHLS_H

#include <vector>
#include <unordered_map>
#include <unordered_set>

class Graph;

std::vector<std::vector<int> > sketch_index(
    const Graph& graph,
    int sketch_node,
    const std::vector<std::unordered_set<int> >& seed_node_sets);

std::vector<int> bfs_to_seed_node(
    const Graph& graph,
    int sketch_node,
    const std::unordered_set<int>& seed_node_set);

std::vector<int> bfs_sketch(
    int sketch_node,
    const std::vector<std::vector<int> >& sketch);

Graph sketchLS(
    const Graph& graph,
    std::vector<int> terminals,
    const std::unordered_map<int, std::vector<std::vector<int> > >& sketches);

std::vector<int> get_path_from_sketch(
    const std::vector<std::vector<int> >& sketch,
    int node);

std::vector<int> concatenate_path(
    std::vector<int> path_to_be_added,
    const std::vector<int>& path_to_add);

#endif // GUARD_SKETCHLS_H

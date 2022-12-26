#ifndef GUARD_SKETCHLS_H
#define GUARD_SKETCHLS_H

#include <vector>
#include <unordered_map>
#include <unordered_set>

class Graph;

/* 事前計算 */
std::vector<int> bfs_to_node_set(
    const Graph& graph,
    int source,
    const std::unordered_set<int>& node_set);

std::vector<int> bfs_to_node_set_avoiding_another_node_set(
    const Graph& graph,
    int source,
    const std::unordered_set<int>& node_set,
    const std::unordered_set<int>& node_set_to_avoid);

std::vector<std::vector<int> > sketch_index(
    const Graph& graph,
    int sketch_node,
    const std::vector<std::unordered_set<int> >& seed_node_sets);

std::vector<std::vector<int> > sketch_index_avoiding_bc_top(
    const Graph& graph,
    int sketch_node,
    const std::vector<std::unordered_set<int> >& seed_node_sets,
    const std::unordered_set<int>& bc_top_nodes);


/* 実行時計算 */
std::vector<int> bfs_sketch(
    int sketch_node,
    const std::vector<std::vector<int> >& sketch);

Graph sketchLS(
    const Graph& graph,
    std::vector<int> terminals,
    const std::unordered_map<int, std::vector<std::vector<int> > >& sketches);

Graph partial_sketchLS(
    const Graph& graph,
    std::vector<int> terminals,
    const std::unordered_map<int, std::vector<std::vector<int> > >& sketches);


/* 下位関数 */
std::vector<int> get_path_from_sketch(
    const std::vector<std::vector<int> >& sketch,
    int node);

std::vector<int> concatenate_path(
    std::vector<int> path_to_be_added,
    const std::vector<int>& path_to_add);

bool has_cycle_for_path(const std::vector<int>& path);

#endif // GUARD_SKETCHLS_H

#ifndef GUARD_SKETCHLS_H
#define GUARD_SKETCHLS_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>

class Graph;

/* 事前計算 */

std::vector<std::vector<int> > sketch_index(
    const Graph& graph,
    int sketch_node,
    const std::vector<std::unordered_set<int> >& seed_node_sets);

std::vector<std::vector<std::vector<int> > > extended_sketch_index(
    const Graph& graph,
    int sketch_node,
    const std::vector<std::unordered_set<int> >& seed_node_sets,
    const std::vector<std::unordered_set<int> >& top_node_sets);

std::unordered_map<int, std::vector<std::vector<int> > > generate_sketches(
    const Graph& graph,
    const std::vector<std::unordered_set<int> >& seed_node_sets);

std::unordered_map<int, std::vector<std::vector<std::vector<int> > > > generate_extended_sketches(
    const Graph& graph,
    const std::vector<std::unordered_set<int> >& seed_node_sets,
    const std::vector<std::unordered_set<int> >& top_node_sets);

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
    const std::unordered_map<int, std::vector<std::vector<int> > >& partial_sketches);


/* extended sketches を扱う関数 */

int get_max_number_of_avoided_top_nodes(
    const std::unordered_map<int, std::vector<std::vector<std::vector<int> > > >& extended_sketches);

std::unordered_map<int, std::vector<std::vector<int> > > get_sketches_from_extended_sketches(
    const std::unordered_map<int, std::vector<std::vector<std::vector<int> > > >& extended_sketches,
    int position);

std::vector<std::unordered_map<int, std::vector<std::vector<int> > > > get_list_of_sketches_from_extended_sketches(
    const std::unordered_map<int, std::vector<std::vector<std::vector<int> > > >& extended_sketches,
    int max_number_of_avoided_top_nodes);


/* 下位関数 */

std::vector<int> get_path_from_sketch(
    const std::vector<std::vector<int> >& sketch,
    int node);

std::vector<int> concatenate_path(
    std::vector<int> path_to_be_added,
    const std::vector<int>& path_to_add);

bool has_cycle_for_path(const std::vector<int>& path);

std::vector<int> get_non_terminal_leaves(
    const std::unordered_map<int, std::vector<int> >& adjacency_list,
    const std::unordered_set<int>& terminal_set);

std::vector<std::vector<int> > get_branches_without_terminal(
    const std::unordered_map<int, std::vector<int> >& adjacency_list,
    const std::unordered_set<int>& terminal_set,
    const std::vector<int>& non_terminal_leaves);

std::vector<std::vector<int> > get_paths_leaf_to_terminal(
    const std::unordered_map<int, std::vector<int> >& adjacency_list,
    const std::unordered_set<int>& terminal_set,
    const std::vector<int>& non_terminal_leaves);

void remove_unnecessary_path_from_ST(Graph& ST, std::vector<int> terminals);

void add_edges_til_terminal_connects_to_ST(Graph& ST, const std::vector<int> path);

std::vector<int> get_shortest_path_from_SPT(
    const std::vector<int>& shortest_path_tree,
    const int& source);

std::vector<int> get_avoided_path_from_APT(
    const std::vector<int>& avoided_path_tree,
    const int& source);

#endif // GUARD_SKETCHLS_H

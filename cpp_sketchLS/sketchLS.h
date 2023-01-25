#ifndef GUARD_SKETCHLS_H
#define GUARD_SKETCHLS_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <string>

class Graph;

/* 事前計算 */
void generate_sketches(
    const Graph& graph,
    const std::vector<std::unordered_set<int> >& seed_node_sets,
    std::vector<std::vector<std::vector<int> > >& sketches);

void generate_extended_sketches(
    const Graph& graph,
    const std::vector<std::unordered_set<int> >& seed_node_sets,
    const std::vector<std::unordered_set<int> >& top_node_sets,
    std::vector<std::vector<std::vector<std::vector<int> > > >& extended_sketches,
    std::vector<double>& precomputation_time_ms_list);

/* 実行時計算 */

std::vector<int> bfs_sketch(
    int sketch_node,
    const std::vector<std::vector<int> >& sketch);

Graph sketchLS(
    const Graph& graph,
    std::vector<int> terminals,
    const std::unordered_map<int, std::vector<std::vector<int> > >& sketches);

/* extended sketches を扱う関数 */

int get_max_number_of_avoided_top_nodes(
    const std::unordered_map<int, std::vector<std::vector<std::vector<int> > > >& extended_sketches);

int get_max_number_of_avoided_top_nodes_vector_ver(
    const std::vector<std::vector<std::vector<std::vector<int> > > >& extended_sketches);

std::unordered_map<int, std::vector<std::vector<int> > > get_sketches_from_extended_sketches(
    const std::unordered_map<int, std::vector<std::vector<std::vector<int> > > >& extended_sketches,
    int position);

std::vector<std::unordered_map<int, std::vector<std::vector<int> > > > get_list_of_sketches_from_extended_sketches(
    const std::unordered_map<int, std::vector<std::vector<std::vector<int> > > >& extended_sketches,
    const std::vector<std::string>& x_list);

std::vector<double> get_avoidability_list(
    const std::vector<std::vector<std::vector<std::vector<int> > > >& extended_sketches,
    const std::vector<std::string>& x_list);


/* 下位関数 */

std::vector<int> get_path_from_sketch(
    const std::vector<std::vector<int> >& sketch,
    int node);

std::vector<int> concatenate_path(
    std::vector<int> path_to_be_added,
    const std::vector<int>& path_to_add);

bool has_cycle_for_path(const std::vector<int>& path);

std::vector<int> get_shortest_path_from_SPT(
    const std::vector<int>& shortest_path_tree,
    const int& source);

std::vector<int> get_avoided_path_from_APT(
    const std::vector<int>& avoided_path_tree,
    const int& source);

#endif // GUARD_SKETCHLS_H

#include <vector>
#include <algorithm>
#include <unordered_map>
#include "graph.h"

using std::vector;
using std::find;
using std::unordered_map;


// 下位関数を前方宣言
vector<int> get_node_list_without_terminals(
    const vector<int>& node_list,
    const vector<int>& terminals);


double evaluate_overlap_ratio(
    const Graph& ST_for_sketchLS,
    const Graph& ST_for_partial_sketchLS,
    const std::vector<int>& terminals)
{
    vector<int> node_list_for_sketchLS = ST_for_sketchLS.get_node_list();
    vector<int> node_list_for_partial_sketchLS = ST_for_partial_sketchLS.get_node_list();

    int total = 0; // ターミナルを除くノードの合計数
    int count = 0; // ターミナルを除くノードでnode_list_for_partial_sketchLSに含まれる合計数
    for (int node : node_list_for_sketchLS) {
        if (find(terminals.begin(), terminals.end(), node) != terminals.end()) {
            continue;
        }
        if (find(node_list_for_partial_sketchLS.begin(), node_list_for_partial_sketchLS.end(), node) != node_list_for_partial_sketchLS.end()) {
            ++count;
        }
        ++total;
    }

    return static_cast<double>(count) / total;
}


double evaluate_sum_of_degree(
    const Graph& graph,
    const Graph& ST,
    const vector<int>& terminals)
{
    double sum_of_degree = 0;

    // 隣接リストを取得
    unordered_map<int, vector<int> > adjacency_list = graph.get_adjacency_list();

    // ターミナル以外のノードを特定
    vector<int> node_list = ST.get_node_list();
    vector<int> node_list_without_terminals
        = get_node_list_without_terminals(node_list, terminals);

    for (const int& node : node_list_without_terminals) {
        sum_of_degree += adjacency_list.at(node).size();
    }

    return sum_of_degree;
}


double evaluate_sum_of_bc(
    const unordered_map<int, double>& bc_map,
    const Graph& ST,
    const vector<int>& terminals)
{
    double sum_of_bc = 0;

    // ターミナル以外のノードを特定
    vector<int> node_list = ST.get_node_list();
    vector<int> node_list_without_terminals
        = get_node_list_without_terminals(node_list, terminals);

    for (const int& node : node_list_without_terminals) {
        sum_of_bc += bc_map.at(node);
    }

    return sum_of_bc;
}


vector<int> get_node_list_without_terminals(
    const vector<int>& node_list,
    const vector<int>& terminals)
{
    vector<int> node_list_without_terminals;

    for (const int& node : node_list) {
        // ターミナルでなければ追加
        if ( find(terminals.begin(), terminals.end(), node) == terminals.end() ) {
            node_list_without_terminals.push_back(node);
        }
    }

    return node_list_without_terminals;
}
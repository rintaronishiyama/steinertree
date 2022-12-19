#include <vector>
#include "graph.h"

using std::vector;

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
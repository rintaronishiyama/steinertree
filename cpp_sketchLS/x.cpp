#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>

using std::cout;
using std::endl; 
using std::vector;
using std::string;
using std::to_string;
using std::unordered_set;

// 避けた bc 上位ノードの個数に対する x の値のリストを返す
vector<string> get_x_list_for_top_node_sets(
    const vector<unordered_set<int> >& top_node_sets)
{
    vector<string> x_list_for_top_node_sets;

    x_list_for_top_node_sets.push_back("0");
    
    for (const unordered_set<int>& top_node_set : top_node_sets) {
        int size = top_node_set.size();
        x_list_for_top_node_sets.push_back( to_string(size) );
    }

    return x_list_for_top_node_sets;
}
#include <vector>
#include <algorithm> // std::sample
#include <iterator>  // std::back_inserter
#include "engine.h"
#include "graph.h"

using std::vector;
using std::sample;
using std::back_inserter;

// node_list からランダムにノードを 1 つ取得
int get_random_element(const vector<int>& vec){
    vector<int> tmp;
    sample(
        vec.begin(),
        vec.end(),
        back_inserter(tmp),
        1,
        engine
    );
    
    return tmp[0];
}

vector<int> decide_terminals(const Graph& graph, int size) {
    const vector<int>& node_list = graph.get_node_list();
    vector<int> terminals;

    sample(
        node_list.begin(),
        node_list.end(),
        back_inserter(terminals),
        size,
        engine
    );

    return terminals;
}
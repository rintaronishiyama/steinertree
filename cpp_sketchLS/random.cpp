#include <vector>
#include <algorithm> // std::sample
#include <iterator>  // std::back_inserter, std::inserter
#include <unordered_set>
#include "engine.h"
#include "graph.h"

using std::vector;
using std::sample;
using std::inserter;
using std::back_inserter;
using std::unordered_set;

// node_list からランダムにノードを 1 つ取得する
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

// グラフ上で指定した数のターミナルをサンプリングする
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


// グラフ上でランダムにノードを選びシードノードとする
// シードノード集合のリストを返す
vector<unordered_set<int> > decide_seed_node_sets(const Graph& graph) {
    // シードノード集合のvector [S1, S2, ..., Sm]
    vector<unordered_set<int> > seed_node_sets;

    const vector<int>& node_list = graph.get_node_list();
    int n = graph.get_number_of_nodes();
    
    // サイズの異なるシードノード集合を追加
    int size = 1;
    while ( size < (n / 2) ) {
        unordered_set<int> tmp;
        // node_list から size 分サンプリング
        sample(
            node_list.begin(),
            node_list.end(),
            inserter(tmp, tmp.begin()),
            size,
            engine
        );
        seed_node_sets.push_back(tmp);
        size *= 2;
    }

    return seed_node_sets;
}

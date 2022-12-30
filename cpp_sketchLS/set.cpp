#include <vector>
#include <unordered_set>

using std::vector;
using std::unordered_set;

// node_list からサイズが max_size となるまでノード集合を取得
// サイズは 2 倍していく
vector<unordered_set<int> > get_node_sets_from_node_list(
    const vector<int>& node_list,
    int max_size)
{
    vector<unordered_set<int> > node_sets;

    int size = 1;
    while (size <= max_size) {
        unordered_set<int> node_set;
        for (int i = 0; i < size; ++i) {
            node_set.insert( node_list.at(i) );
        }
        node_sets.push_back(node_set);
        size *= 2;
    }

    return node_sets;
}
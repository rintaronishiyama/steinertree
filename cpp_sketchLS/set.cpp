#include <vector>
#include <unordered_set>

using std::vector;
using std::unordered_set;

// node_list からサイズが max_size となるまでノード集合を取得
// サイズはインクリメントしていく
vector<unordered_set<int> > get_node_sets_from_node_list_by_increment(
    const vector<int>& node_list,
    int max_size)
{
    vector<unordered_set<int> > node_sets;

    // 1 ~ max_size までのサイズのノード集合を追加
    for (int size = 1; size < (max_size + 1); ++size) {
        unordered_set<int> node_set;

        for (int i = 0; i < size; ++i) {
            node_set.insert( node_list.at(i) );
        }

        node_sets.push_back(node_set);
    }

    return node_sets;
}


// node_list からサイズが max_size となるまでノード集合を取得
// サイズはノード数の0.1%ずつ増加
// 最後に max_size のノード集合を追加
vector<unordered_set<int> > get_node_sets_from_node_list_by_add(
    const vector<int>& node_list,
    int max_size)
{
    vector<unordered_set<int> > node_sets;

    // max_size を 10 で割って切り捨て
    int interval = max_size / 10;

    // max_size が
    // 割り切れる場合 　: interval, interval x 2, ..., interval x 9
    // 割り切れない場合 : interval, interval x 2, ..., interval x 10
    // のサイズのノード集合を追加
    int size = interval;
    while (size < max_size) {
        unordered_set<int> node_set;
        for (int i = 0; i < size; ++i) {
            node_set.insert( node_list.at(i) );
        }
        node_sets.push_back(node_set);
        size += interval;
    }

    // max_size のノード集合を追加
    unordered_set<int> node_set;
    for (int i = 0; i < max_size; ++i) {
        node_set.insert( node_list.at(i) );
    }
    node_sets.push_back(node_set);

    return node_sets;
}


// node_list からサイズが max_size となるまでノード集合を取得
// サイズを2倍していく
// 最後に max_size のノード集合を追加
vector<unordered_set<int> > get_node_sets_from_node_list_by_multiply(
    const vector<int>& node_list,
    int max_size)
{
    vector<unordered_set<int> > node_sets;

    // max_size より小さい 2 の倍数のサイズまでのノード集合を追加
    int size = 1;
    while (size < max_size) {
        unordered_set<int> node_set;
        for (int i = 0; i < size; ++i) {
            node_set.insert( node_list.at(i) );
        }
        node_sets.push_back(node_set);
        size *= 2;
    }

    // max_size のノード集合を追加
    unordered_set<int> node_set;
    for (int i = 0; i < max_size; ++i) {
        node_set.insert( node_list.at(i) );
    }
    node_sets.push_back(node_set);

    return node_sets;
}

// max_size のノード集合のみを追加
vector<unordered_set<int> > get_node_sets_from_node_list_by_all(
    const vector<int>& node_list,
    int max_size)
{
    vector<unordered_set<int> > node_sets;

    // max_size のノード集合を追加
    unordered_set<int> node_set;
    for (int i = 0; i < max_size; ++i) {
        node_set.insert( node_list.at(i) );
    }
    node_sets.push_back(node_set);

    return node_sets;
}
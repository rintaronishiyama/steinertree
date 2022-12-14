#include <vector>
#include <cmath>   // std::ceil
#include <unordered_map>
#include <utility>

using std::vector;
using std::ceil;
using std::unordered_map;
using std::pair;

/* ノードリストを分割 */
vector<vector<int> > divide_node_list(
    const std::vector<int>& node_list,
    double length_to_divide)
{
    vector<vector<int> > devided_list_of_node_list;
    int times_of_division = static_cast<int>(1 / length_to_divide);
    int node_list_length = node_list.size();
    
    for (int i = 0; i < times_of_division; ++i) {
        vector<int>::const_iterator begin = node_list.cbegin() + ceil(node_list_length * length_to_divide * i);
        vector<int>::const_iterator end = node_list.cbegin() + ceil(node_list_length * length_to_divide * (i + 1) );
        
        vector<int> tmp(begin, end);

        devided_list_of_node_list.push_back(tmp);
    }

    return devided_list_of_node_list;
}

/* sketches を分割されたノードリストに合わせて分割 */
vector<unordered_map<int, vector<vector<int> > > > divide_sketches(
    const unordered_map<int, vector<vector<int> > >& sketches,
    double length_to_divide,
    const vector<int>& node_list_sorted)
{
    using Sketch = vector<vector<int> >;
    using Sketches = unordered_map<int, Sketch>;

    vector<vector<int> > divided_list_of_node_list = divide_node_list(node_list_sorted, length_to_divide);
    vector<Sketches> divided_list_of_sketches;

    for (const vector<int>& node_list : divided_list_of_node_list) {
        Sketches tmp_sketches;
        
        for (int node : node_list) {
            tmp_sketches[node] = sketches.at(node);
        }
        divided_list_of_sketches.push_back(tmp_sketches);
    }

    return divided_list_of_sketches;
}
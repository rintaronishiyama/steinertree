#include <vector>
#include <string>
#include <iostream>

using std::cout;
using std::endl; 
using std::vector;
using std::string;
using std::to_string;

// 避けた bc 上位ノードの個数に対する x の値のリストを返す
vector<string> get_x_list_for_avoided_bc_top_nodes(
    int max_number_of_avoided_bc_top_nodes)
{
    vector<string> x_list_for_avoided_bc_top_nodes;

    x_list_for_avoided_bc_top_nodes.push_back("Original");
    
    if (max_number_of_avoided_bc_top_nodes != 0) {
        int tmp = 1;
        while (tmp <= max_number_of_avoided_bc_top_nodes) {
            x_list_for_avoided_bc_top_nodes.push_back( to_string(tmp) );
            tmp *= 2;
        }
    }

    return x_list_for_avoided_bc_top_nodes;
}


// sketch の保持を限定したノードの範囲に対する x の値のリストを返す
vector<string> get_x_list_for_limit_range(
    double length_to_divide)
{
    vector<string> x_list_for_limit_range;

    x_list_for_limit_range.push_back("NoDivision");

    int length_of_range_percent = static_cast<int>(length_to_divide * 100);
    int times_of_division = static_cast<int>(100 / length_of_range_percent);

    for (int i = 0; i < times_of_division; ++i) {
        int bottom = length_of_range_percent * i;
        int top = length_of_range_percent * (i + 1);

        string range = to_string(bottom) + "-" + to_string(top);

        x_list_for_limit_range.push_back(range);
    }

    return x_list_for_limit_range;
}
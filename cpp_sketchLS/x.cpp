#include <vector>
#include <string>

using std::vector;
using std::string;
using std::to_string;

vector<string> get_x_list_for_list_of_sketches(
    int max_number_of_avoided_bc_top_nodes)
{
    vector<string> x_list_for_list_of_sketches;

    x_list_for_list_of_sketches.push_back("original");
    
    if (max_number_of_avoided_bc_top_nodes != 0) {
        int tmp = 1;
        while (tmp <= max_number_of_avoided_bc_top_nodes) {
            x_list_for_list_of_sketches.push_back( to_string(tmp) );
            tmp *= 2;
        }
    }

    return x_list_for_list_of_sketches;
}
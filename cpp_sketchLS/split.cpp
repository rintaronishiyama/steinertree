#include <vector>
#include <string>
#include <sstream>

using std::vector;
using std::string;
using std::stringstream;

vector<string> split(const string &str, char delimiter) {
    vector<string> elems;
    stringstream ss(str);
    string item;
    while (getline(ss, item, delimiter)) {
    if (!item.empty()) {
            elems.push_back(item);
        }
    }
    return elems;
}
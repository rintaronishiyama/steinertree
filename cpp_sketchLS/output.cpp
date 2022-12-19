#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

void output_terminals(const vector<int>& terminals) {
    cout << "temirnals : ";
    for (const int& terminal : terminals) {
        if ( terminal == terminals.front() ) {
            cout << "[";
        }
        if ( terminal == terminals.back() ) {
            cout << " " << terminal << " ]";
            break;
        }
        cout << " " << terminal << ",";
    }
    cout << endl;
}
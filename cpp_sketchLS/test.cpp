#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <utility>
#include <unordered_set>
#include <unordered_map>
#include <map>

using namespace std;


int main(int argc, char* argv[])
{
    double a = 0.05;
    int b = 5;
    string c = to_string(b) + "cc";

    cout << a * 101 << endl;
    cout << to_string( a * 100 ) << endl;
    cout << to_string( static_cast<int>(a * 100) ) << endl;
    cout << c << endl;
}
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <unordered_set>

using namespace std;


int main(int argc, char* argv[])
{
    unordered_set<int> test;
    test.insert(1);
    if (!(test.count(0))) {
        cout << "work" << endl;
    } else {
        cout << "not work" << endl;
    }
}
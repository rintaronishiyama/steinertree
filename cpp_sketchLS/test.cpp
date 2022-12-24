#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

using namespace std;

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    fs::create_directories("./data");
    ofstream ofs("./data/data.txt");
    ofs << "aa" << endl;
    fs::create_directories("./data");
    ofstream ofs2("./data/data.txt");
    ofs2 << "bb" << endl;
}
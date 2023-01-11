#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <utility>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <chrono>

using namespace std;
namespace ch = std::chrono;


int main(int argc, char* argv[])
{
    ch::system_clock::time_point start, end;
    start = ch::system_clock::now();
    #pragma omp parallel for
    for (int i = 0; i < 1000000; ++i) {
        cout << "aa is empty" << endl;
    }
    end = ch::system_clock::now();

    auto time = ch::duration_cast<ch::milliseconds>(end - start).count();
    cout << time << endl;
}
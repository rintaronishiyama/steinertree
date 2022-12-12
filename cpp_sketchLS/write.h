#ifndef GUARD_WRITE_H
#define GUARD_WRITE_H

#include <vector>
#include <unordered_map>
#include <string>

class Graph;

void write_sketches(
    std::string file_path,
    const std::unordered_map<int, std::vector<std::vector<int> > >& sketches);

void write_graph(std::string file_path, const Graph& graph);
#endif // GUARD_WRITE_H
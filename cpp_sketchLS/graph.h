#ifndef GUARD_GRAPH_H
#define GUARD_GRAPH_H

#include <vector>
#include <unordered_map>
#include <unordered_set>

class Graph{
    private :
        std::vector<int> node_list;
        int number_of_nodes = 0;
        std::unordered_map<int, std::vector<int> > adjacency_list; // ノードid -> 隣接ノードのvector

    public :
        // getter 関数
        const std::vector<int>& get_node_list() const;
        const std::unordered_map<int, std::vector<int> >& get_adjacency_list() const;
        int get_number_of_nodes() const;
        int get_number_of_edges() const;
        const std::vector<int>& get_node_list_sorted_by_degree();

        // グラフの操作
        void add_edge(int n1, int n2);
        void delete_edge(int n1, int n2);
        void add_path(const std::vector<int>& path);
        void delete_path(const std::vector<int>& path);

        // グラフの分析
        bool is_connected() const;
        bool has_cycle() const;
        bool dfs(
            int source,
            std::vector<int>& visited_nodes,
            std::vector<int>& unvisited_nodes) const;
        std::vector<int> bfs_to_node_set(
            int source,
            const std::unordered_set<int>& node_set) const;
        std::vector<int> bfs_to_node_set_avoiding_another_node_set(
            int source,
            const std::unordered_set<int>& node_set,
            const std::unordered_set<int>& node_set_to_avoid) const;
        std::vector<int> find_shortest_path(int source, int target) const;
        std::vector<int> bfs_from_source_node_set(const std::unordered_set<int>& source_node_set) const;
        std::vector<int> bfs_from_source_node_set_avoiding_another_node_set(
            const std::unordered_set<int>& source_node_set,
            const std::unordered_set<int>& node_set_to_avoid) const;

        // グラフの出力
        void output_graph() const;
};

#endif // GUARD_GRAPH_H
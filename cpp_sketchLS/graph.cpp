#include <vector>
#include <unordered_map>
#include <utility>      // std::pair
#include <algorithm>    // std::find, std::reverse, std::sort
#include <queue>        // std::priority_queue
#include <functional>   // std::greater
#include <iostream>
#include <iterator>     // std::back_inserter;
#include <queue>
#include <stack>
#include "graph.h"
#include "random.h"

using std::vector;
using std::unordered_map;
using std::pair;
using std::find;
using std::priority_queue;
using std::greater;
using std::reverse;
using std::make_pair;
using std::sort;
using std::cout;
using std::endl;
using std::sample;
using std::back_inserter;
using std::queue;
using std::stack;

const int INF = 1e9;

/* getter 関数 */
std::vector<int> Graph::get_node_list() const{
    vector<int> node_list;
    // ノードidのvectorを作成
    for (const pair<int, vector<int> >& item : this->adjacency_list) {
        node_list.push_back(item.first);
    }

    return node_list;
}

const std::unordered_map<int, std::vector<int> >& Graph::get_adjacency_list() const{
    return this->adjacency_list;
}

int Graph::get_number_of_nodes() const{
    return get_node_list().size();
}

int Graph::get_number_of_edges() const{
    int number_of_edges = 0;
    // ノードが持つエッジの数を合計する
    for (const pair<int, vector<int> >& item : this->adjacency_list) {
        number_of_edges += item.second.size();
    }
    
    return number_of_edges;
}


// ノードを次数の降順にソートしたリストを返す
vector<int> Graph::get_node_list_sorted_by_degree(){
    vector<int> node_list_sorted_by_degree = this->get_node_list();
    sort(
        node_list_sorted_by_degree.begin(),
        node_list_sorted_by_degree.end(),
        [this](const int& left, const int& right){
            // 隣接ノードの数 (次数) を比較 
            return this->get_adjacency_list().at(left).size() > this->get_adjacency_list().at(right).size();
        }
    );

    return node_list_sorted_by_degree;
}

/* グラフの操作 */
void Graph::add_edge(int n1, int n2){
    // セルフループは追加しない
    if (n1 == n2) {
        throw "Self-loop is not acceptable!";
    }

    // すでにあるエッジは追加しない (多重辺の禁止)
    unordered_map<int, vector<int> >& dic = this->adjacency_list;
    vector<int>::iterator find_itr1
        = find(dic[n1].begin(), dic[n1].end(), n2);
    vector<int>::iterator find_itr2
        = find(dic[n2].begin(), dic[n2].end(), n1);
    if ( ( find_itr1 != dic[n1].end() ) || ( find_itr2 != dic[n2].end() ) ) {
        throw "Multiple edges is not acceptable!";
    }

    // adjacency_list に n1 -> n2, n2 -> n1 を追加(無向のため)
    this->adjacency_list[n1].push_back(n2);
    this->adjacency_list[n2].push_back(n1);
}

void Graph::delete_edge(int n1, int n2) {
    unordered_map<int, vector<int> >& dic = this->adjacency_list;

    // n1, n2 の隣接ノードの vector から n2, n1 を削除
    vector<int>::iterator find_itr1
        = find(dic[n1].begin(), dic[n1].end(), n2);
    vector<int>::iterator find_itr2
        = find(dic[n2].begin(), dic[n2].end(), n1);
    if ( find_itr1 != dic[n1].end() ) {
        dic[n1].erase(find_itr1);
    }
    if ( find_itr2 != dic[n2].end() ) {
        dic[n2].erase(find_itr2);
    }

    // n1, n2 の隣接ノードがなければ adjacency_list から削除
    if ( dic[n1].empty() ) {
        dic.erase(n1);
    }
    if ( dic[n2].empty() ) {
        dic.erase(n2);
    }
}

void Graph::add_path(const vector<int>& path) {
    for (int i = 0; i < (path.size() - 1); ++i) {
        this->add_edge(path[i], path[i + 1]);
    }
}


/* グラフの分析 */
// distances や p を unordered_map に対応させないと部分グラフには使えない
std::vector<int> Graph::find_shortest_path(int source, int target) const {
    // 省略のため using
    using Pair = pair<int, int>;
    
    // getter 関数呼び出し
    const unordered_map<int, vector<int> >& graph = this->adjacency_list;
    int n = get_number_of_nodes();

    vector<int> distances(n, INF);

    // 直前のノードを記録する配列
    vector<int> p(n, -1);

	// 「現時点での最短距離, 頂点」の順に取り出す priority_queue
	// デフォルトの priority_queue は降順に取り出すため std::greater を使う
	priority_queue<Pair, vector<Pair>, greater<Pair> > q;
	// q.emplace((distances[source] = 0), source);
    q.push(make_pair((distances[source] = 0), source));

    while ( !q.empty() ) {
        const int distance = q.top().first;
        const int from = q.top().second;
        q.pop();

        // 最短距離でなければ処理しない
        if (distances[from] < distance) {
            continue;
        }

        // 現在の頂点からの各エッジについて
        for (const int& neighbor : graph.at(from) /*graph が const 参照のため at()*/) {
            // neighbor までの新しい距離
            const int d = distances[from] + 1;

            // d が現在の距離より小さければ更新
            if (d < distances[neighbor]) {
                // 直前の頂点を記録
                p[neighbor] = from;
                
                // q.emplace((distances[neighbor] = d), neighbor);
                q.push(make_pair((distances[neighbor] = d), neighbor));
            }
        }
    }


    // target に到達できれば最短経路を再構築する
    vector<int> shortest_path;
    if (distances[target] != INF) {
        for (int i = target; i != -1; i = p[i]) {
            shortest_path.push_back(i);
        }
        
        reverse( shortest_path.begin(), shortest_path.end() );
    }

    return shortest_path;
}

// 連結であるか確認. 連結なら true そうでなければ false
bool Graph::is_connected() const {
    /* ランダムなノードを 1 つ取得 */
    int random_node = get_random_element( this->get_node_list() );

    /* 取得したノードから BFS. すべてのノードを訪問したら連結と判定 */
    vector<int> visited_nodes;
    queue<int> nodes_to_search;
    visited_nodes.push_back(random_node);
    nodes_to_search.push(random_node);
    while ( !nodes_to_search.empty() ) {
        vector<int> neighbors = this->adjacency_list.at(nodes_to_search.front());
        nodes_to_search.pop();
        for (int neighbor : neighbors) {
            vector<int>::iterator find_itr
                = find(visited_nodes.begin(), visited_nodes.end(), neighbor);
            if ( find_itr == visited_nodes.end() ) {
                visited_nodes.push_back(neighbor);
                nodes_to_search.push(neighbor);
            }
        }
    }
    vector<int> node_list = this->get_node_list();

    return node_list.size() == visited_nodes.size();
}

// 閉路を持つか確認. 持てば true そうでなければ false
bool Graph::has_cycle() const {
    // node_list からランダムに選ぶ
    int source_node = get_random_element( this->get_node_list() );
    vector<int> visited_nodes;
    vector<int> unvisited_nodes = this->get_node_list();
    while ( !dfs(source_node, visited_nodes, unvisited_nodes) ) {
        if ( visited_nodes.size() == this->get_node_list().size() ) {
            return false;
        }
        // 未訪問のノードからランダムに選ぶ
        source_node = get_random_element(unvisited_nodes);
    }

    return true;
}

// source から dfs. 訪問したノードのリストを引数としてとり, そこに入れてく
bool Graph::dfs(int source, vector<int>& visited_nodes, vector<int>& unvisited_nodes) const {
    stack<pair<int, int> > nodes_to_search; // <node, previous> のペア

    visited_nodes.push_back(source);
    unvisited_nodes.erase( find(unvisited_nodes.begin(), unvisited_nodes.end(), source) );
    nodes_to_search.push({source, -1});
    while ( !nodes_to_search.empty() ) {
        int searching_node = nodes_to_search.top().first;
        int previous_node = nodes_to_search.top().second;
        nodes_to_search.pop();
        for (int neighbor : this->adjacency_list.at(searching_node) ) {
            vector<int>::iterator find_itr
                = find(visited_nodes.begin(), visited_nodes.end(), neighbor);
            if ( find_itr == visited_nodes.end() ){
                visited_nodes.push_back(neighbor);
                unvisited_nodes.erase( find(unvisited_nodes.begin(), unvisited_nodes.end(), neighbor) );
                nodes_to_search.push({neighbor, searching_node});
                continue;
            }
            if (*find_itr == previous_node) {
                continue;
            }
            return true;
        }
    }

    return false;
}
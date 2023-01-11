#include <vector>
#include <unordered_map>
#include <unordered_set>
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
using std::unordered_set;
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

// const 参照のノードリストを返す
const vector<int>& Graph::get_node_list() const{
    return this->node_list;
}


// const 参照の隣接リストを返す
const std::unordered_map<int, std::vector<int> >& Graph::get_adjacency_list() const{
    return this->adjacency_list;
}


// ノード数を返す
int Graph::get_number_of_nodes() const{
    return this->number_of_nodes;
}


// エッジ数を返す
int Graph::get_number_of_edges() const{
    int number_of_edges = 0;
    // ノードが持つエッジの数を合計する
    for (const pair<int, vector<int> >& item : this->adjacency_list) {
        number_of_edges += item.second.size();
    }
    number_of_edges /= 2; // 無向で同じ辺が2回カウントされるため

    return number_of_edges;
}


// ノードを次数の降順にソートしたリストを返す
const vector<int>& Graph::get_node_list_sorted_by_degree(){
    vector<int>& node_list_sorted_by_degree = this->node_list;
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
        return;
    }

    // すでにあるエッジは追加しない (多重辺の禁止)
    unordered_map<int, vector<int> >& dic = this->adjacency_list;
    vector<int>::const_iterator find_itr1 = find(dic[n1].cbegin(), dic[n1].cend(), n2);
    vector<int>::const_iterator find_itr2 = find(dic[n2].cbegin(), dic[n2].cend(), n1);
    if ( ( find_itr1 != dic[n1].cend() ) || ( find_itr2 != dic[n2].cend() ) ) {
        return;
    }

    // adjacency_list に n1 -> n2, n2 -> n1 を追加(無向のため)
    this->adjacency_list[n1].push_back(n2);
    this->adjacency_list[n2].push_back(n1);

    // node_list になければ追加, number_of_nodes をインクリメント
    if (find(this->node_list.begin(), this->node_list.end(), n1) == this->node_list.end()) {
        this->node_list.push_back(n1);
        ++this->number_of_nodes;
    }
    if (find(this->node_list.begin(), this->node_list.end(), n2) == this->node_list.end()) {
        this->node_list.push_back(n2);
        ++this->number_of_nodes;
    }
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

    // n1, n2 の隣接ノードがなければ adjacency_list と node_list から削除, number_of_nodes をデクリメント
    if ( dic[n1].empty() ) {
        dic.erase(n1);
        this->node_list.erase(find(this->node_list.begin(), this->node_list.end(), n1));
        --this->number_of_nodes;
    }
    if ( dic[n2].empty() ) {
        dic.erase(n2);
        this->node_list.erase(find(this->node_list.begin(), this->node_list.end(), n2));
        --this->number_of_nodes;
    }
}

void Graph::add_path(const vector<int>& path) {
    for (int i = 0; i < (path.size() - 1); ++i) {
        this->add_edge(path[i], path[i + 1]);
    }
}

void Graph::delete_path(const vector<int>& path) {
    for (int i = 0; i < (path.size() - 1); ++i) {
        this->delete_edge(path[i], path[i + 1]);
    }
}


/* グラフの分析 */

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

// source から node_set 内のいずれかのノードにつくまで BFS
// source から node_set 内で最も近いノードまでの最短経路を返す
vector<int> Graph::bfs_to_node_set(
    int source,
    const unordered_set<int>& node_set) const
{
    vector<int> shortest_path;
    if (node_set.count(source)) { // source が node_set に含まれているとき
        shortest_path.push_back(source);
        return shortest_path;
    }

    int n = this->get_number_of_nodes();
    vector<int> dist(n, 1e9);
    vector<int> pre(n, -1); // 経路として見たとき一つ前の頂点
    queue<int> que;
    que.push(source);
    dist[source] = 0;

    int target = -1; // node_set に属する頂点のうち, source から一番近いもの

    while (!que.empty()) {
        // 現在頂点
        int from = que.front();
        que.pop();

        // node_set に属する頂点に辿り着いたかどうか
        bool find_node_set = false;

        for (const int& to : this->get_adjacency_list().at(from)) { // from の隣接頂点を見ていく
            if (dist[to] != 1e9) continue; // 訪問済みなら飛ばす

            dist[to] = dist[from] + 1;     // 現在頂点から+1した距離を記録
            pre[to] = from;                // 現在頂点を1つ前の頂点として記録

            if (node_set.count(to)) {      // to が node_set に属するならそこで終了
                target = to;
                find_node_set = true;
                break;
            } 

            que.push(to);
        }

        if (find_node_set) break;
    }

    if (target == -1) {
        throw "cant find target in bfs";
    }

    // target から巻き戻し, 最短経路を構築
    for (int i = target; i != -1; i = pre[i]) {
        shortest_path.push_back(i);
    }
    reverse( shortest_path.begin(), shortest_path.end() );

    return shortest_path;
}


// source から node_set 内のいずれかのノードにつくまで BFS
// node_set_to_avoid 内のいずれかのノードにヒットしたら飛ばす
// source から node_set 内で最も近いノードまでの
// node_set_to_avoid を避けた最短経路を返す
std::vector<int> Graph::bfs_to_node_set_avoiding_another_node_set (
    int source,
    const unordered_set<int>& node_set,
    const unordered_set<int>& node_set_to_avoid) const
{
    vector<int> shortest_path;
    if (node_set.count(source)) { // source が node_set に含まれているとき
        shortest_path.push_back(source);
        return shortest_path;
    }

    int n = this->get_number_of_nodes();
    vector<int> dist(n, 1e9);
    vector<int> pre(n, -1); // 経路として見たとき一つ前の頂点
    queue<int> que;
    que.push(source);
    dist[source] = 0;

    int target = -1; // node_set に属する頂点のうち, source から一番近いもの

    while (!que.empty()) {
        // 現在頂点
        int from = que.front();
        que.pop();

        // node_set に属する頂点に辿り着いたかどうか
        bool find_node_set = false;

        for (const int& to : this->get_adjacency_list().at(from)) { // from の隣接頂点を見ていく
            if (dist[to] != 1e9) continue;                                           // 訪問済みなら飛ばす
            if ( (node_set_to_avoid.count(to)) && !(node_set.count(to)) ) continue;  // 避けるノード集合にあり, 目的のノード集合になければ飛ばす

            dist[to] = dist[from] + 1;     // 現在頂点から+1した距離を記録
            pre[to] = from;                // 現在頂点を1つ前の頂点として記録

            if (node_set.count(to)) {      // to が node_set に属するならそこで終了
                target = to;
                find_node_set = true;
                break;
            } 

            que.push(to);
        }

        if (find_node_set) break;
    }

    if (target == -1) { // targetが見つからなければ空の vector を返す
        return shortest_path;
    }

    // target から巻き戻し, 最短経路を構築
    for (int i = target; i != -1; i = pre[i]) {
        shortest_path.push_back(i);
    }
    reverse( shortest_path.begin(), shortest_path.end() );

    return shortest_path;
}

// source から target への最短経路を返す
// bfs_to_node_set を利用
vector<int> Graph::find_shortest_path(int source, int target) const {
    unordered_set<int> node_set;
    node_set.insert(target);
    return this->bfs_to_node_set(source, node_set);
}

// ソースノード集合を最初にqueueに入れた状態でBFS
// 一つ前のノードを記録したpreを返す 一つ前のノードがなければ-1
// 必然的にソースノード集合のノードのみ pre が -1 になる
vector<int> Graph::bfs_from_source_node_set(
    const unordered_set<int>& source_node_set) const
{
    int n = this->get_number_of_nodes();
    vector<int> dist(n, 1e9);
    vector<int> pre(n, -1); // 経路として見たとき一つ前の頂点
    queue<int> que;

    // ソースノード集合を全てqueueに入れる
    for (const int& source_node : source_node_set) {
        que.push(source_node);
        dist[source_node] = 0;
    }


    while (!que.empty()) {
        // 現在頂点
        int from = que.front();
        que.pop();

        for (const int& to : this->get_adjacency_list().at(from)) { // from の隣接頂点を見ていく
            if (dist[to] != 1e9) continue; // 訪問済みなら飛ばす

            dist[to] = dist[from] + 1;     // 現在頂点から+1した距離を記録
            pre[to] = from;                // 現在頂点を1つ前の頂点として記録

            que.push(to);
        }
    }

    return pre;
}


// ソースノード集合を最初にqueueに入れた状態でBFS
// 一つ前のノードを記録したpreを返す 一つ前のノードがなければ -2, ソースノードのpreは -1
vector<int> Graph::bfs_from_source_node_set_avoiding_another_node_set(
    const unordered_set<int>& source_node_set,
    const unordered_set<int>& node_set_to_avoid) const
{
    int n = this->get_number_of_nodes();
    vector<int> dist(n, 1e9);
    vector<int> pre(n, -2); // 経路として見たとき一つ前の頂点
    queue<int> que;

    // ソースノード集合を全てqueueに入れる
    for (const int& source_node : source_node_set) {
        que.push(source_node);
        dist[source_node] = 0;
        pre[source_node] = -1;
    }


    while (!que.empty()) {
        // 現在頂点
        int from = que.front();
        que.pop();

        for (const int& to : this->get_adjacency_list().at(from)) { // from の隣接頂点を見ていく
            if (dist[to] != 1e9) continue; // 訪問済みなら飛ばす
            if ( node_set_to_avoid.count(to) ) continue;  // 避けるノード集合にあれば飛ばす

            dist[to] = dist[from] + 1;     // 現在頂点から+1した距離を記録
            pre[to] = from;                // 現在頂点を1つ前の頂点として記録

            que.push(to);
        }
    }

    return pre;
}


/* グラフの出力 */
void Graph::output_graph() const {
    unordered_map<int, vector<int> > adjacency_list = this->get_adjacency_list();

    for (const pair<int, vector<int> >& item : adjacency_list) {
        cout << item.first << " : ";

        for (const int& neighbor : item.second ) {
            cout << neighbor << " ";
        }

        cout << endl;
    }
}
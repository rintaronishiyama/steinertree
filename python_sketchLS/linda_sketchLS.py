import sys
import networkx as nx
import random
from collections import deque
import time
from numpy import maximum
import timeout_decorator
from retry import retry
import copy
from networkx.algorithms.approximation import steiner_tree
import matplotlib.pyplot as plt

def linda_sketchls(graph, terminal, denominator):
    sketch = []
    BFS = []
    S_cover = []
    T = nx.Graph()
    node = list(graph.nodes)
    node_number = graph.number_of_nodes()
    node_other = copy.copy(node) #nodeの浅いコピー, 参照がnode_otherに渡される
    terminal_number = len(terminal)
    landmarks = [] # ターミナル毎のlandmarkのリスト [ターミナル1のlandmarkリスト, ターミナル2の...]
    seed_nodes = []
    # ターミナルをnode_otherから除く
    for i in range(len(terminal)):
        if node_other.count(terminal[i]) != 0:
            node_other.remove(terminal[i])
    # 要素数が2倍されていく seed nodes の集合 S1, S2, ... ,Sm を決定 (seed_nodesにすべて追加)
    # seed_nodes = [S1, S2, S3, S4, ...] S1 = [node], S2 = [node, node]
    size = 1
    while size  < (node_number / denominator):
        added_seed_nodes = random.sample(node_other, size)
        seed_nodes.append(added_seed_nodes)
        size *= 2
    # sketchをターミナル数分生成
    for i in range(terminal_number):
        sketch.append(list(linda_sketch_index(graph, terminal[i], seed_nodes, landmarks, i)))
    
    # print('seed nodes')
    # for i in seed_nodes :
    #     print(i)
    # print('landmarks')
    # for i in landmarks :
    #     print(i)
    # print('sketch')
    # for i in sketch :
    #     print(i)

    # BFSをターミナル数分生成
    # BFS の中身は BFS で訪れたノードのリストである nodes をターミナル数分配列にしたもの
    # e.g., BFS = [nodes, nodes, nodes, ...]. nodes = [2, 4, 9, ...]
    for i in range(terminal_number):
        visited_nodes = linda_bfs(terminal[i], sketch[i])
        BFS.append(visited_nodes)
    
    # print('BFS')
    # for i in BFS :
    #     print(i)

    iter_num = 0    
    
    F = [] # F の中身は 訪問したノードをターミナル毎にリストにしたもの e.g., [[node, node,...], [node, node,...], ...] 
    
    while BFS:
        v = BFS[iter_num][0]
        if len(F) <= iter_num:
            F.append([v])
        else:
            F[iter_num].append(v)   
        for j in range(terminal_number):
            if j != iter_num:
                if len(F) <= j:
                    continue
                else:
                    neighbors = list(graph.neighbors(v)) # vの隣接ノードのリスト
                    index_neighbor = 0
                    count = 0
                    n = 0
                    # 他のターミナルの訪問済みノードリストの中に v の隣接ノードがあれば count を増やす
                    while count == 0 and index_neighbor < len(neighbors):
                        count += F[j].count(neighbors[index_neighbor])
                        index_neighbor += 1 
                    if count != 0:
                        n = neighbors[index_neighbor - 1] # -1 することで F に含まれる v の隣接ノードに戻す

                        cycle = []
                        # 「注目しているターミナルから v への最短経路」と「v に隣接し j の訪問済みノードである n と jの最短経路」をあわせた経路
                        # ↓ sketchの経路情報を使えばいいのでは？
                        # p_tmp = nx.shortest_path(graph, source=terminal[iter_num], target=v) + nx.shortest_path(graph,source=n, target=terminal[j])
                        for i in sketch[iter_num] :
                            if i.count(v) != 0 :
                                path1 = i[ 0 : i.index(v) + 1] # terminal[iter_num] から v までの経路を sketch から取得
                                break
                        for i in sketch[j] :
                            if i.count(n) != 0 :
                                path2 = i[i.index(n) :  : -1] # n から terminal[j] までの経路を sketch から取得 
                                break
                        p_tmp = path1 + path2
                        # 暫定シュタイナー木である　T に p_tmp の経路を加えた時、閉路ができないか確認
                        Tcopy = T.copy()
                        nx.add_path(Tcopy, p_tmp)
                        cycle = nx.cycle_basis(Tcopy, terminal[iter_num])
                        if len(cycle) != 0: # 閉路があるならjの次のループへ
                            continue

                        nx.add_path(T, p_tmp)

                        # S_cover中に注目しているターミナルが含まれなければ追加
                        if S_cover.count(terminal[iter_num]) == 0:
                            S_cover.append(terminal[iter_num])
                        # S_cover中に注目ターミナルと一緒に経路を追加したjがなければ追加
                        if S_cover.count(terminal[j]) == 0:
                            S_cover.append(terminal[j])
                    continue
            # j == iter_num ならjの次のループへ

        if len(BFS[iter_num]) > 0:
            BFS[iter_num].pop(0) #注目ターミナルのBFSの初めの要素を削除
        
        # S_coverがターミナル数以上, かつ T が連結なら while を抜け T を返す
        if len(S_cover) >= terminal_number and nx.is_connected(T):
            break
        
        # iter_num が最後(ターミナルの最終イテレータ)でなければインクリメント. そうでなければ 0 へ戻す (round-robinになる部分)
        if iter_num != terminal_number - 1:
            iter_num += 1
        else:
            iter_num = 0

    return T

# sketch を生成, path を返し, landmarksを参照でいじる
def linda_sketch_index(graph, terminal_node, seed_nodes, landmarks, index):
    SPT = []
    tmp_path = []
    shortest_path = 0
    path = []
    landmark_index = 0

    for i in range(len(seed_nodes)):
        # terminal_node と Si の最短経路を探し, Si内で最も近いノード(landmark)を見つける
        for j in range(len(seed_nodes[i])):
            tmp_path = nx.shortest_path(graph, source=terminal_node, target=seed_nodes[i][j])
            if len(tmp_path) == 0 :
                landmark_index = j
                break
            if j == 0:
                shortest_path = len(tmp_path)
                # ↓これ　landmark_index = j では？
                # landmark_index = i
                landmark_index = j
            else:
                if shortest_path > len(tmp_path):
                    shortest_path = len(tmp_path)
                    # ↓これも　landmark_index = j では？
                    # landmark_index = i
                    landmark_index = j
        # path = [S0の最短経路, S1の最短経路, ...]
        path.append(nx.shortest_path(graph, source=terminal_node, target=seed_nodes[i][landmark_index]))


        if i == 0: # 初めはそのまま追加
            landmarks.append([seed_nodes[i][landmark_index]])
        elif landmarks[index].count(seed_nodes[i][landmark_index]) == 0: # それ以降はSiのlandmarkがすでに追加されてるものでない場合に追加
            landmarks[index].append(seed_nodes[i][landmark_index])
        i += 1

    return path


# 幅優先探索
# def bfs(G, source, landmarks):
#     neighbors = G.neighbors
#     visited = {source}
#     depth_limit = len(G)
#     landmark = copy.copy(landmarks)
#     queue = deque([(source, depth_limit, neighbors(source))])

#     while landmark:
#         parent, depth_now, children = queue[0]
#         try:
#             child = next(children)
#             if child not in visited:
#                 yield parent, child
#                 visited.add(child)
#                 if landmark.count(child) != 0:
#                     landmark.remove(child)
#                     queue.append((child, depth_now - 1, neighbors(child)))
#                     continue
#                 if depth_now > 1:
#                     queue.append((child, depth_now - 1, neighbors(child)))
#         except StopIteration:
#             queue.popleft()
def linda_bfs(source, sketch_of_source) :
    visited_nodes = [source]
    len_list = []
    for i in sketch_of_source :
        len_list.append(len(i))
    max_len = max(len_list)
    for i in range(1, max_len) :
        for j in range(len(sketch_of_source)) :
            if (i + 1) > len(sketch_of_source[j]) :
                continue
            if not sketch_of_source[j][i] in visited_nodes :
                visited_nodes.append(sketch_of_source[j][i])
    
    return visited_nodes


# グラフ読み込み
f = open('../dataset/ego-facebook.txt', 'r')
# f = open('../dataset/graph.txt', 'r')
datalist = f.readlines()
f.close()
G = nx.Graph()
for data in datalist :
    edge = data.split()
    G.add_edge( int(edge[0]), int(edge[1]) )
print('Complete loading input file\n')

# ランダムグラフ生成
# n = 30
# edge_probability = 0.1
# # seed = 1125 # シード値（適当な値でいい)。これが変わると生成するグラフも変わる。逆に同じ値なら同じグラフを生成。
# # G = nx.fast_gnp_random_graph(n, edge_probability, seed = seed)
# G = nx.fast_gnp_random_graph(n, edge_probability)
# while not nx.is_connected(G) :
#     G = nx.fast_gnp_random_graph(n, edge_probability)
# nx.draw_networkx(G)
# plt.savefig('graph.png')

# ターミナルの決定 
# number_of_terminals = 5
# terminals = random.sample(G.nodes, number_of_terminals)
# print('terminals :', terminals)
# print('Complete sampling terminals\n')
terminals = [3515, 2161, 1979]


# Steiner Tree 演算 (Networkx)
# ST = steiner_tree(G, terminals)
# print('Complete calculating a Steiner Tree (Networkx)\n')

# Steiner Tree 演算 (SketchLS)
SketchLS_ST = linda_sketchls(G, terminals, 2)
print('Complete approximating a Steiner Tree (SketchLS)\n')

# グラフ出力
# plt.subplot(2, 2, 1)
# plt.title('Graph')
# nx.draw_networkx(G)
# print('Complete drawing graph')

# plt.subplot(2, 2, 2)
# plt.title('Steiner Tree')
# nx.draw_networkx(ST)
# print('Complete drawing Steiner Tree (Networkx)')

# plt.subplot(2, 2, 3)
# plt.title('SketchLS')
# nx.draw_networkx(SketchLS_ST)
# print('Complete drawing Steiner Tree (SketchLS)')

# plt.savefig('compare.png')
# print("Complete saving figure")

plt.title('SketchLS')
nx.draw_networkx(SketchLS_ST)
plt.savefig('SketchLS.png')
print("Complete saving figure")

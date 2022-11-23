""" method to get memory footprint """
from __future__ import print_function
from sys import getsizeof, stderr
from itertools import chain
from collections import deque
try:
    from reprlib import repr
except ImportError:
    pass

def total_size(o, handlers={}, verbose=False):
    """ Returns the approximate memory footprint an object and all of its contents.

    Automatically finds the contents of the following builtin containers and
    their subclasses:  tuple, list, deque, dict, set and frozenset.
    To search other containers, add handlers to iterate over their contents:

        handlers = {SomeContainerClass: iter,
                    OtherContainerClass: OtherContainerClass.get_elements}

    """
    dict_handler = lambda d: chain.from_iterable(d.items())
    all_handlers = {tuple: iter,
                    list: iter,
                    deque: iter,
                    dict: dict_handler,
                    set: iter,
                    frozenset: iter,
                   }
    all_handlers.update(handlers)     # user handlers take precedence
    seen = set()                      # track which object id's have already been seen
    default_size = getsizeof(0)       # estimate sizeof object without __sizeof__

    def sizeof(o):
        if id(o) in seen:       # do not double count the same object
            return 0
        seen.add(id(o))
        s = getsizeof(o, default_size)

        if verbose:
            print(s, type(o), repr(o), file=stderr)

        for typ, handler in all_handlers.items():
            if isinstance(o, typ):
                s += sum(map(sizeof, handler(o)))
                break
        return s

    return sizeof(o)
""" method to get memory footprint """










from functools import partial
import sys
from tabnanny import check
import networkx as nx
import random
from collections import deque
import time
from numpy import maximum, number
import timeout_decorator
from retry import retry
import copy
from networkx.algorithms.approximation import steiner_tree
import matplotlib.pyplot as plt
import math
import os
import numpy as np

""" 事前計算 """

# 近傍の上位ノードを決定
def decide_nearest_top_node(graph, i, top_nodes) :
    nearest_top_node = -1
    bfs = []
    bfs.append([i]) # sourceのみの配列を追加
    current_level = 0 # bfsの現時点の深さ
    flag = 0
    while flag == 0 :
        neighbors = []
        for j in range( len(bfs[current_level]) ) :
            for k in graph.neighbors(bfs[current_level][j]):
                if top_nodes.count(k) != 0 :
                    nearest_top_node = k
                    flag = 1
                    return nearest_top_node
                neighbors.append(k)   
        bfs.append(neighbors)
        current_level += 1
    
    if nearest_top_node == -1 :
        print("Error : could not find nearest top node")
        sys.exit(1)

# sketch生成
def precomputation(graph, top_rates, sketch, memory_usage, precomputation_time) :
    # 計測開始
    start = time.perf_counter()

    # seed_nodes 決定
    node = list(graph.nodes)
    n = graph.number_of_nodes()
    seed_nodes = []
    size = 1
    while size < (n / 2) : # denominator = 2 とする
        added_seed_nodes = random.sample(node, size)
        seed_nodes.append(added_seed_nodes)
        size *= 2
    print("Complete decideing seed nodes")

    # sketch 初期化
    for i in range(n) :
        sketch.append(-1)
    
    # sketch生成準備
    degree = sorted(graph.degree, key=lambda top_rates: top_rates[1], reverse=True) # 次数の降順にソート, ペア (ノードid, 次数)　の配列
    checklist = [] # どの割合まで完了したか確認する配列
    for i in top_rates :
        checklist.append( math.floor(i * n) )
    count = 0

    # sketch生成
    for (i, j) in degree :
        # 指定の割合分生成しているか確認
        check_index = checklist.count("done")
        if check_index != len(checklist) :
            if not count < checklist[check_index] :
                end = time.perf_counter()
                precomputation_time[top_rates[check_index]] += end - start
                memory_usage[top_rates[check_index]] += calc_MB( total_size(sketch) )
                print("Complete generating", top_rates[check_index] * 100,"% sketch")
                checklist[check_index] = "done"

        sketch[i] = list(sketch_index(graph, i, seed_nodes))
        count += 1
        print("Sketching top", count, "node")
    end = time.perf_counter()
    print("Complete generating all sketch")

    # allの分append
    precomputation_time[1] = end - start
    memory_usage[1] = calc_MB(total_size(sketch))





""" SketchLS """

# SketchLS を実行
def sketchls(graph, terminal, sketch):
    terminal_number = len(terminal)
    sketch_of_terminals = []
    for i in terminal :
        sketch_of_terminals.append(sketch[i])

    # BFSをターミナル数分生成
    # BFS の中身は BFS で訪れたノードのリストである nodes をターミナル数分配列にしたもの
    # e.g., BFS = [nodes, nodes, nodes, ...]. nodes = [2, 4, 9, ...]
    BFS = []
    for i in range(terminal_number):
        visited_nodes = bfs_sketch(terminal[i], sketch_of_terminals[i])
        BFS.append(visited_nodes)

    iter_num = 0    
    
    F = [] # F の中身は 訪問したノードをターミナル毎にリストにしたもの e.g., [[node, node,...], [node, node,...], ...] 
    
    S_cover = []
    T = nx.Graph()
    while BFS:
        # BFS[iter_num]が空の配列であればiter_numを変更
        while not BFS[iter_num] :
            if iter_num != terminal_number - 1:
                iter_num += 1
            else:
                iter_num = 0
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
                        for i in sketch_of_terminals[iter_num] :
                            if i.count(v) != 0 :
                                path1 = i[ 0 : i.index(v) + 1] # terminal[iter_num] から v までの経路を sketch から取得
                                break
                        for i in sketch_of_terminals[j] :
                            if i.count(n) != 0 :
                                path2 = i[i.index(n) :  : -1] # n から terminal[j] までの経路を sketch から取得 
                                break
                        p_tmp = path1 + path2
                        # ここから
                        flag = True
                        for i in p_tmp :
                            if p_tmp.count(i) != 1 :
                               flag = False 
                        if not flag :
                            continue
                        # ここまで怪しい, うまくいきはしても理由がわからない

                        # 暫定シュタイナー木である　T に p_tmp の経路を加えた時、閉路ができないか確認
                        Tcopy = T.copy()
                        nx.add_path(Tcopy, p_tmp)
                        cycle = nx.cycle_basis(Tcopy, terminal[iter_num])
                        if len(cycle) != 0: # 閉路があるならjの次のループへ
                            continue
                        print(p_tmp, "is added")
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
# def sketch_index(graph, sketch_node, seed_nodes, landmarks, index): # landmarksいらない, indexもいらない
def sketch_index(graph, sketch_node, seed_nodes):
    tmp_path = []
    shortest_path = 0
    path = []
    landmark_index = 0

    for i in range(len(seed_nodes)):
        # sketch_node と Si の最短経路を探し, Si内で最も近いノード(landmark)を見つける
        for j in range(len(seed_nodes[i])):
            tmp_path = nx.shortest_path(graph, source=sketch_node, target=seed_nodes[i][j])
            if len(tmp_path) == 0 :
                landmark_index = j
                break
            if j == 0:
                shortest_path = len(tmp_path)
                landmark_index = j
            else:
                if shortest_path > len(tmp_path):
                    shortest_path = len(tmp_path)
                    landmark_index = j
        # path = [S0の最短経路, S1の最短経路, ...]
        path.append(nx.shortest_path(graph, source=sketch_node, target=seed_nodes[i][landmark_index]))

    return path

# 幅優先探索
def bfs_sketch(source, sketch_of_source) :
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





""" 実行 """

def my_algorithm(graph, terminal, partial_sketch) :
    pair_of_terminal = [] # [ [変更前のターミナル1, 変更後のターミナル1], [変更前のターミナル2, 変更後のターミナル2], ...]
    for i in range(len(terminal)) :
        if isinstance(partial_sketch[terminal[i]], int) :
            pair_of_terminal.append([ terminal[i], partial_sketch[terminal[i]] ])
        else :
            pair_of_terminal.append( [terminal[i], terminal[i]] )
    tmp_terminal = []
    for i in pair_of_terminal :
        tmp_terminal.append(i[1])

    tmp_terminal = list( set(tmp_terminal) ) # 近傍の上位ノードが同じであれば重複を削除

    T = nx.Graph()
    if len(tmp_terminal) != 1: # tmp_terminal(置き換え後のターミナル)が1個でなければ sketchls
        print("tmp_terminal :", tmp_terminal)
        T = sketchls(graph, tmp_terminal, partial_sketch)


    for i in pair_of_terminal :
        if i[0] != i[1] :
            print(i[0], "-", i[1], "shortest path added")
            path_to_add = nx.shortest_path(graph, source=i[1], target=i[0])
            nx.add_path(T, path_to_add) # [変更前のターミナル1, 変更後のターミナル1]を経路として追加

    # 求めたST中にターミナルが全て含まれるか確認
    for i in terminals :
        if list(T.nodes).count(i) == 0 :
            print('Error : My algo ST does not involve all terminals')
            sys.exit(1)
    print('My algo ST involves all terminals')

    # 求めたSTの全ての辺が元のグラフに含まれるか確認
    for (i, j) in list(T.edges) :
        if i not in graph.neighbors(j) :
            print('Error : My algo ST involves edge missing in original graph')
            sys.exit(1)
    print('All edges of my algo ST are involved in original graph\n')

    return T



def execution(graph, top_rates, sketch, partial_sketches, terminals, run_time, approx_errors, _STs, index) :
    # my_algorithm を実行 (partial_sketchesを利用)
    my_algo_STs = [[] for i in range(len(top_rates))] 
    for i in range( len(top_rates) ) :
        print(top_rates[i], "sketch begins")
        start = time.perf_counter()
        my_algo_STs[i] = my_algorithm(graph, terminals, partial_sketches[top_rates[i]])
        if my_algo_STs[i].size() == 0 :
            print("no ST")
            sys.exit(1)
        
        end = time.perf_counter()

        run_time[i] += end - start

    # sketchLS を実行 (sketchを利用)
    print("all sketch begins")
    start = time.perf_counter()
    sketchLS_ST = sketchls(graph, terminals, sketch)
    sketchLS_ST_size = sketchLS_ST.size()
    end = time.perf_counter()

    if len(run_time) == len(top_rates) :
        run_time.append(end - start)
    else :
        run_time[len(top_rates)] += end - start

    # approx error 計算
    for i in range( len(top_rates) ) :
        currentST_size = my_algo_STs[i].size()
        approx_error = (currentST_size - sketchLS_ST_size) / sketchLS_ST_size
        approx_errors[i] += approx_error
    
    # ST を記録
    _STs[index] = my_algo_STs.copy()
    _STs[index].append(sketchLS_ST)






""" linda skstchls begin """

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
    

    # BFSをターミナル数分生成
    # BFS の中身は BFS で訪れたノードのリストである nodes をターミナル数分配列にしたもの
    # e.g., BFS = [nodes, nodes, nodes, ...]. nodes = [2, 4, 9, ...]
    for i in range(terminal_number):
        visited_nodes = linda_bfs(terminal[i], sketch[i])
        BFS.append(visited_nodes)
    

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

""" linda sketchls end """





""" ターミナルの決定 """
def decide_terminals(graph, number_of_terminals) :
    random.seed() # 乱数初期化
    terminals = random.sample(graph.nodes, number_of_terminals)
    print('terminals :', terminals)
    print('Complete sampling terminals\n')
    
    return terminals





""" メモリの計算 """
def calc_MB(memory) :
    return memory / (1024 * 1024)




""" sketch を保存 """
def save_sketch(sketch, dataset_dirpath) :
    f = open(dataset_dirpath + 'sketch.txt', 'w')

    for i in sketch :
        for j in i :
            for k in j :
                f.write(str(k) + " ")
            f.write("next ")
        f.write("end\n")
    
    f.close()





""" sketch を読み込み """
def load_sketch(sketch, dataset_dirpath) :
    # sketch を読み込み
    f = open(dataset_dirpath + 'sketch.txt', 'r')

    lines = f.readlines()
    for line in lines :
        datalist = line.split()
        tmp_sketch = []
        tmp_shortest_path = []
        for data in datalist :
            if data == "end" :
                break
            if data == "next" :
                tmp_sketch.append(tmp_shortest_path)
                tmp_shortest_path = []
                continue
            tmp_shortest_path.append(int(data))
        sketch.append(tmp_sketch)
    
    f.close()





""" 結果を保存 """
def save_result(dict, dataset_dirpath, name_of_dict) :
    path = dataset_dirpath + name_of_dict + ".txt"
    f = open(path, "w")

    for key, value in dict.items() :
        f.write(str(key) + " " + str(value) + "\n")

    f.close()





""" 結果を読み込み """
def load_result(dict, dataset_dirpath, name_of_dict) :
    path = dataset_dirpath + name_of_dict + ".txt"
    f = open(path, "r")

    lines = f.readlines()
    for line in lines :
        data = line.split()
        dict[float(data[0])] = float(data[1])

    f.close()





""" main """
# データセット一覧を表示
dataset_listdir = os.listdir("../dataset/")
if len(dataset_listdir) == 0 :
    print("There is no dataset.")
else :
    print("there are following precomputation results")
    for i in dataset_listdir :
        print(i)
print()

# グラフ読み込み
dataset = input("enter dataset without extention(.) : ")
dataset_filepath = "../dataset/" + dataset + ".txt" # データセットそのもののファイルパス
if not os.path.isfile(dataset_filepath) :
    print("there is no such file")
    sys.exit(1)
f = open(dataset_filepath, 'r')
lines = f.readlines()
f.close()
G = nx.Graph()
for line in lines :
    edge = line.split()
    if edge[0] != edge[1] : # セルフループは追加しない, 多重辺はnetworkxの方で消してくれる
        G.add_edge( int(edge[0]), int(edge[1]) )
if not nx.is_connected(G) : # 非連結なグラフだと Steiner Tree は求められないので異常終了
    print("This dataset is not connected")
    sys.exit(1)
print('Complete loading input file\n')

# データセットに関する実験結果のディレクトリパス
dataset_dirpath = "./" + dataset + "/"

# モード選択
print("seletct mode")
print("1 : precomputation")
print("2 : execution")
print("3 : degree")
mode = input()
print()

if mode == "1" :
    """ 事前計算 """
    # データセットのディレクトリ作成
    if os.path.isdir(dataset_dirpath) :
        print(dataset_dirpath, "already exists. No need for precomputation!")
        sys.exit(0)
    else :
        print(dataset_dirpath, "is not found, so this is created now.")
        os.makedirs(dataset_dirpath) # ディレクトリ作成

    # 上位ノードの割合決定 (事前計算時は 0.001 (0.1%) 毎に 0.999 (99.9%) まで評価を取る)
    top_rates = []
    if len(G.nodes) < 1000 : # コードテストに向けた小さいデータセット用
        for i in range(1, 10) :
            top_rates.append( round(i * 0.1, 1) )
    else :
        for i in range(1, 1000) :
            top_rates.append(round(i * 0.001, 3))

    memory_usage = {}
    precomputation_time = {}
    for i in top_rates :
        memory_usage[i] = 0
        precomputation_time[i] = 0

    sketch = []

    precomputation(G, top_rates, sketch, memory_usage, precomputation_time)
    




    """ txtファイルに結果を保存 """
    save_sketch(sketch, dataset_dirpath)
    save_result(memory_usage, dataset_dirpath,"memory_usage")
    save_result(precomputation_time, dataset_dirpath, "precomputation_time")





    """ pngファイルに結果を保存 """
    # 事前計算時間記録
    x_for_precomputation_time = list( precomputation_time.keys() )
    y_for_precomputation_time = list( precomputation_time.values() )
    for i in range( len(x_for_precomputation_time) ) :
        x_for_precomputation_time[i] *= 100
    plt.ylabel("precomputation time[s]")
    plt.xlabel("top nodes[%]")
    plt.plot(x_for_precomputation_time, y_for_precomputation_time)
    plt.grid()
    plt.savefig(dataset_dirpath + "precomputation_time.png")
    plt.close()
    
    # メモリ使用量記録
    x_for_memory_usage = list( memory_usage.keys() )
    y_for_memory_usage = list( memory_usage.values() )
    for i in range( len(x_for_memory_usage) ) :
        x_for_memory_usage[i] *= 100
    plt.ylabel("memory usage[MB]")
    plt.xlabel("top nodes[%]")
    plt.plot(x_for_memory_usage, y_for_memory_usage)
    plt.grid()
    plt.savefig(dataset_dirpath + "memory_usage.png")
    plt.close()
    
    # メモリ使用量に対する事前計算時間を記録
    plt.ylabel("precomputation time[s]")
    plt.xlabel("memory usage[MB]")
    plt.scatter(y_for_memory_usage, y_for_precomputation_time)
    plt.grid()
    plt.savefig(dataset_dirpath + "precomputation_time_for_memory_usage.png")
    plt.close()

elif mode == "2" :
    """ 実行 """
    # データセットのディレクトリがあるか確認 (事前計算しているか確認)
    if not os.path.isdir(dataset_dirpath) :
        print(dataset_dirpath, "is not found. Please precompute before execution!")
        sys.exit(1)
    
    # interval 分増やすか, それぞれで取るか選択
    print("select increment or each")
    print("1 : increment")
    print("2 : each")
    selection = input()
    print()

    interval = float(input("enter interval of top nodes more than 0.001 :")) # トップの割合を刻む間隔
    times = int(input("enter times of repeating interval :")) # 間隔を何回繰り返すか
    print()
    top_rates = [] # トップの割合
    for i in range(1, times + 1) :
        top_rates.append(round(i * interval, 3))

    pair_of_conditions = ""
    execution_dirpath = ""

    if selection == "1" :
        # データセット内条件毎のパス決定
        pair_of_conditions = "increment" + str(interval) + "times" + str(times)
        execution_dirpath = dataset_dirpath + pair_of_conditions + "/" # ./データセット名/increment○○times○○/
    elif selection == "2" :
        # データセット内条件毎のパス決定
        pair_of_conditions = "each" + str(interval) + "times" + str(times)
        execution_dirpath = dataset_dirpath + pair_of_conditions + "/" # ./データセット名/each○○times○○/
    else :
        print("select right one")
        sys.exit(1)
    
    os.makedirs(execution_dirpath, exist_ok=True) #　条件毎のディレクトリ作成
    os.makedirs(execution_dirpath + "result_png/ST_png", exist_ok=True)
    os.makedirs(execution_dirpath + "result_png/ST_degree_png", exist_ok=True)
    os.makedirs(execution_dirpath + "result_txt", exist_ok=True)

    # 事前計算結果読み込み
    sketch = []
    memory_usage = {}
    precomputation_time = {}
    load_sketch(sketch, dataset_dirpath)
    load_result(memory_usage, dataset_dirpath,"memory_usage")
    load_result(precomputation_time, dataset_dirpath,"precomputation_time")
    
    # partial_sketches 決定
    partial_sketches = {}
    sketch_nodes = [] # sketch を持つノードidを記録した配列の配列

    if selection == "1" :
        degree = sorted(G.degree, key=lambda top_rates: top_rates[1], reverse=True) # 次数の降順にソート　(ノードid, 次数)　の配列
        checklist = []
        for i in top_rates :
            checklist.append( math.floor(i * G.number_of_nodes()))
        print(checklist)
        count = 0
        tmp_sketch_nodes = []
        for (i, j) in degree :
            check_index = checklist.count("done")
            if check_index != len(checklist) :
                if not count < checklist[check_index] :
                    sketch_nodes.append(tmp_sketch_nodes.copy())
                    checklist[check_index] = "done"
            else :
                break
            tmp_sketch_nodes.append(i)
            count += 1
    if selection == "2" :
        degree = sorted(G.degree, key=lambda top_rates: top_rates[1], reverse=True) # 次数の降順にソート　(ノードid, 次数)　の配列
        checklist = []
        for i in top_rates :
            checklist.append( math.floor(i * G.number_of_nodes()))
        count = 0
        sketch_nodes = [[] for i in range(len(top_rates))] # sketch を持つノードidを記録した配列の配列
        for (i, j) in degree :
            check_index = checklist.count("done")
            if check_index != len(checklist) :
                if not count < checklist[check_index] :
                    checklist[check_index] = "done"
            else :
                break    
            sketch_nodes[check_index].append(i)
            count += 1

    for i in range(len(top_rates)) :
        partial_sketches[top_rates[i]] = [-1 for i in range(G.number_of_nodes())]
        for j in sketch_nodes[i] :
            partial_sketches[top_rates[i]][j] = (sketch[j].copy())

    for i in partial_sketches.values() :
        print(i.count(-1))

    number_of_terminals = int(input("enter number of terminals :")) # ターミナル数

    number_for_averaging = 10 # 平均をとるための試行回数

    search_time = [0 for i in range(len(top_rates))]
    run_time = [0 for i in range(len(top_rates))]
    approx_errors = [0 for i in range(len(top_rates))]
    STs = [[] for i in range(number_for_averaging)]
    terminals_list = []

    # 実行
    for i in range(0, number_for_averaging) :
        print("---------------- trial", i + 1, "----------------")
        
        # ターミナルの決定
        terminals = decide_terminals(G, number_of_terminals)
        terminals_list.append(terminals)

        # partial_sketchesの処理 (近傍ノード決定)
        for j in range( len(partial_sketches) ) :
            for terminal in terminals :
                if partial_sketches[top_rates[j]][terminal] == -1 :
                    start = time.perf_counter()
                    partial_sketches[top_rates[j]][terminal] = decide_nearest_top_node(G, terminal, sketch_nodes[j])
                    end = time.perf_counter()
                    search_time[j] += end - start

        # my_algorithm と sketchlsを実行
        execution(G, top_rates, sketch, partial_sketches, terminals, run_time, approx_errors, STs, i)

    # 最小のSTとSketchLSのSTのサイズを取得
    min_ST_size_list = []
    sketchLS_size_list = []
    for i in range(len(STs)) :
        min = 10000
        for j in STs[i] :
            if j.size() < min :
                min = j.size()
        min_ST_size_list.append(min)
        sketchLS_size_list.append(STs[i][-1].size())

    # 平均化
    for i in range(len(top_rates)) :
        search_time[i] /= number_for_averaging
        run_time[i] /= number_for_averaging
        approx_errors[i] *= 100
        approx_errors[i] /= number_for_averaging
    run_time[len(top_rates)] /= number_for_averaging # sketchLSの分
    average_min_ST_size = np.average(min_ST_size_list)
    average_sketchLS_size = np.average(sketchLS_size_list)

    # 事前計算時間とメモリ使用量を条件に合わせて配列化
    precomputation_time_list = []
    memory_usage_list = []
    if selection == "1" :
        for i in top_rates :
            precomputation_time_list.append(precomputation_time[i])
            memory_usage_list.append(memory_usage[i])
    elif selection == "2" :
        for i in top_rates :
            if i == top_rates[0] :
                precomputation_time_list.append(precomputation_time[i])
                memory_usage_list.append(memory_usage[i])
            else :
                precomputation_time_list.append(precomputation_time[i] - precomputation_time[round(i - interval, 3)])
                memory_usage_list.append(memory_usage[i] - memory_usage[round(i - interval,3)])
    precomputation_time_list.append(precomputation_time[1])
    memory_usage_list.append(memory_usage[1])





    """ pngファイルに結果を保存 """
    top_rates_str = []
    for i in top_rates :
        top_rates_str.append(str(round(i * 100, 3)))
    top_rates_str.append("all")
    # ST の図を保存
    for i in range( len(top_rates_str) ) :
        if top_rates_str[i] == "all" :
            file_name = "all"
        else :
            file_name = top_rates_str[i] + "%ST"
        plt.title(file_name)
        nx.draw_networkx(STs[0][i])
        file_name += ".png"
        file_path = execution_dirpath + "result_png/ST_png/" + file_name
        plt.savefig(file_path)
        plt.close()
    
    # ST の次数を視覚化
    nodelist = []
    sum_nodes = []

    # ST 中のノードのリストを取得
    for i in range(len(top_rates_str)) :
        tmp = list(STs[0][i].nodes)
        nodelist.append(tmp.copy())
        sum_nodes += tmp.copy()
    sum_nodes.sort()
    sum_nodes = list(set(sum_nodes))
    print(sum_nodes)
    sum_nodes = sorted(sum_nodes, key=lambda x: G.degree[x], reverse=True)
    count_degree = []
    
    # STs[0] に出現するノードのカウント
    for i in range(len(sum_nodes)) :
        tmp = []
        for j in range(len(nodelist)) :
            if nodelist[j].count(sum_nodes[i]) != 0 :
                tmp.append(1)
            else :
                tmp.append(0)
        count_degree.append(tmp.copy())

    # sum_nodes 中最大の次数を持つノードを特定
    max_degree = 0
    for i in range(len(list(G.nodes))) :
        if max_degree < G.degree[list(G.nodes)[i]] :
            max_degree = G.degree[list(G.nodes)[i]]
    
    # bottomline の準備
    bottomline = []
    for i in range(len(top_rates_str)) :
        bottomline.append(0)
    
    # plot

    for i in range(len(count_degree)) :
        if i == 0 :
            plt.bar(top_rates_str, count_degree[i], edgecolor="black", label=str(sum_nodes[i]), color=[G.degree[sum_nodes[i]]/max_degree, 0, 1 - G.degree[sum_nodes[i]]/max_degree])
        else :
            plt.bar(top_rates_str, count_degree[i], edgecolor="black", label=str(sum_nodes[i]), color=[G.degree[sum_nodes[i]]/max_degree, 0, 1 - G.degree[sum_nodes[i]]/max_degree], bottom=bottomline)
        for j in range(len(count_degree[i])) :
            bottomline[j] += count_degree[i][j]
        if i == len(count_degree) - 1 : # 最後に保存
            plt.xticks(rotation=45)
            plt.savefig(execution_dirpath + "result_png/ST_degree_png/compare_degree_rb.png")
            plt.close()


    for i in range(len(bottomline)) :
        bottomline[i] = 0
    print("bottomline", bottomline)
    
    for i in range(len(count_degree)) :
        if i == 0 :
            plt.bar(top_rates_str, count_degree[i], edgecolor="black", label=str(sum_nodes[i]))
        else :
            plt.bar(top_rates_str, count_degree[i], edgecolor="black", label=str(sum_nodes[i]), bottom=bottomline)
        for j in range(len(count_degree[i])) :
            bottomline[j] += count_degree[i][j]
        if i == len(count_degree) - 1 : # 最後に保存
            plt.xticks(rotation=45)
            plt.savefig(execution_dirpath + "result_png/ST_degree_png/compare_degree.png")
            plt.close()


    # 2 つの値の棒グラフ
    plt.ylabel("time[s]")
    plt.xlabel("top nodes[%]")
    # 1つ目の棒グラフ
    plt.bar(top_rates_str[:-1], run_time[:-1], color='b', width=0.3, label='execution', align="center", bottom=search_time)
    # 2つ目の棒グラフ
    plt.bar(top_rates_str[:-1], search_time, color='g', width=0.3, label='search', align="center")
    # 凡例
    plt.legend(loc=2)
    plt.savefig(execution_dirpath + "result_png/compare_execution.png")
    plt.close()

    plt.ylabel("precomputation time[s]")
    plt.xlabel("top nodes[%]")
    plt.bar(top_rates_str, precomputation_time_list)
    plt.savefig(execution_dirpath + "result_png/precomputation_time.png")
    plt.close()

    plt.ylabel("memory usage[MB]")
    plt.xlabel("top nodes[%]")
    plt.bar(top_rates_str, memory_usage_list)
    plt.savefig(execution_dirpath + "result_png/memory_usage.png")
    plt.close()

    plt.ylabel("run time[s]")
    plt.xlabel("top nodes[%]")
    plt.bar(top_rates_str, run_time)
    plt.savefig(execution_dirpath + "result_png/runtime.png")
    plt.close()

    plt.ylabel("approx error[%]")
    plt.xlabel("top nodes[%]")
    plt.bar(top_rates_str[:-1], approx_errors)
    plt.savefig(execution_dirpath + "result_png/approx_error.png")
    plt.close()

    plt.ylabel("run time[s]")
    plt.xlabel("memory usage[MB]")
    plt.scatter(memory_usage_list, run_time)
    plt.grid()
    plt.savefig(execution_dirpath + "result_png/run_time_for_memory_usage.png")
    plt.close()

    plt.ylabel("approx error[%]")
    plt.xlabel("memory usage[MB]")
    plt.scatter(memory_usage_list[:-1], approx_errors)
    plt.grid()
    plt.savefig(execution_dirpath + "result_png/approx_error_for_memory_usage.png")
    plt.close()





    """ テキストファイルに結果を保存 """
    # ターミナルを保存
    f = open(execution_dirpath + 'result_txt/terminals.txt', 'w')
    for i in range( len(terminals_list) ) :
        line = "trial " +  str(i+1) + " : " + str(terminals_list[i])
        f.write(line + "\n")
    f.close()

    # 評価を保存
    f = open(execution_dirpath + 'result_txt/execution_result.txt', 'w')

    f.write("search time[s]\n")
    for i in range( len(top_rates) ) :
        line = top_rates_str[i] + " : " + str(search_time[i])
        f.write(line + "\n")

    f.write("run time[s]\n")
    for i in range( len(top_rates_str) ):
        line = top_rates_str[i] + " : " + str(run_time[i])
        f.write(line + "\n")

    f.write("approx error[%]\n")
    for i in range( len(top_rates_str[:-1]) ):
        line = top_rates_str[i] + " : " + str(approx_errors[i])
        f.write(line + "\n")

    f.write("ST size\n")
    f.write("average min ST size : " + str(average_min_ST_size) +"\n")
    f.write("average sketchLS ST size : " + str(average_sketchLS_size) + "\n")
    f.write("number of legend (sum_nodes length): " +  str(len(sum_nodes)))
    f.close()

elif mode == "3" :
    f = open(dataset_dirpath + 'degree.txt', 'w')
    degree = sorted(G.degree, key=lambda top_rates: top_rates[1], reverse=True) # 次数の降順にソート　(ノードid, 次数)　の配列
    dict_degree = dict(degree)
    plt.bar(*np.unique(list(dict_degree.values()), return_counts=True))
    plt.title("Degree histogram")
    plt.xlabel("Degree")
    plt.ylabel("# of Nodes")
    plt.savefig(dataset_dirpath + "degree_distribution.png")
    for (i,j) in degree:
        line = str(i) + " : " + str(j)
        f.write(line + "\n")
    f.close()

else :
    print("select right mode")
    sys.exit(1)
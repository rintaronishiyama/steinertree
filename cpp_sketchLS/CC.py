import networkx as nx
import matplotlib.pyplot as plt

""" 読み込み """
dataset_name = input("enter the dataset name : ")
dataset_path = "../dataset/" + dataset_name + ".txt"
f = open(dataset_path, 'r')
lines = f.readlines()
f.close()

""" グラフ生成 """
G = nx.Graph()
for line in lines :
    data = line.split()
    if data[0] == data[1] :
        continue
    G.add_edge(int(data[0]), int(data[1]))

""" CCを演算 """
CC_dict = nx.closeness_centrality(G)

""" 降順にソート """
CC_dict = dict( sorted(CC_dict.items(), key = lambda CC : CC[1], reverse=True) )

""" CCを記録 """
result_path = dataset_name + "/CC.txt"
f = open(result_path, 'w')
for node, cc in CC_dict.items() :
    f.write(str(node) + " " + str(cc) + "\n")
f.close()

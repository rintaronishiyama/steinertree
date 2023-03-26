import networkx as nx
import os
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

""" ノード数取得 """
n = nx.number_of_nodes(G)

f = open("./" + dataset_name + "/node.txt", 'w')
f.write(str(n))
f.close()
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

""" DCを演算 """
DC_dict = nx.degree_centrality(G)

""" 降順にソート """
DC_dict = dict( sorted(DC_dict.items(), key = lambda DC : DC[1], reverse=True) )

""" DCを記録 """
result_path = dataset_name + "/DC.txt"
f = open(result_path, 'w')
for node, dc in DC_dict.items() :
    f.write(str(node) + " " + str(dc) + "\n")
f.close()

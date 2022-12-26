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

""" BCを演算 """
BC_dict = nx.betweenness_centrality(G)

""" 降順にソート """
BC_dict = sorted(BC_dict.items(), key = lambda BC : BC[1], reverse=True)

""" BCを記録 """
result_path = dataset_name + "/BC.txt"
f = open(result_path, 'w')
for node, bc in BC_dict.items() :
    f.write(str(node) + " " + str(bc) + "\n")
f.close()

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

""" BCを記録 """
result_path = dataset_name + "/BC.txt"
f = open(result_path, 'w')
for node, bc in BC_dict.items() :
    f.write(str(node) + " " + str(bc) + "\n")
f.close()

""" BCの分布図作成 """
plt.hist( list( BC_dict.values() ) , 1000)
figure_path = dataset_name + "/BC_distribution.png"
plt.savefig(figure_path)
plt.close()

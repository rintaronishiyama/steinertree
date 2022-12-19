import networkx as nx
import matplotlib.pyplot as plt

graph_name = input("Enter graph name : ")
ST_txt_path = "./" + graph_name + "/SteinerTree.txt"
G = nx.Graph()
f = open(ST_txt_path, 'r')
lines = f.readlines()
f.close()
for line in lines :
    nodes = line.split()
    for i in range(2, len(nodes)) :
        G.add_edge( int(nodes[0]), int(nodes[i]))
nx.draw_networkx(G)
save_path = "./" + graph_name + "/SteinerTree.png"
plt.savefig(save_path)
plt.close()
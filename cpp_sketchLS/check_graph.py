import networkx as nx
import matplotlib.pyplot as plt

ST_txt_path = "./ego-facebook/SteinerTree5.000000%to10.000000%.txt"
G = nx.Graph()
f = open(ST_txt_path, 'r')
lines = f.readlines()
f.close()
for line in lines :
    nodes = line.split()
    for i in range(2, len(nodes)) :
        G.add_edge( int(nodes[0]), int(nodes[i]))
nx.draw_networkx(G)
save_path = "./ego-facebook/SteinerTree5.000000%to10.000000%.png"
plt.savefig(save_path)
plt.close()
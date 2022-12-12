import networkx as nx
import matplotlib.pyplot as plt

f = open("../dataset/test_dataset.txt", 'r')
lines = f.readlines()
f.close()
G = nx.Graph()
for line in lines :
    edge = line.split()
    if edge[0] != edge[1] : # セルフループは追加しない, 多重辺はnetworkxの方で消してくれる
        G.add_edge( int(edge[0]), int(edge[1]) )

nx.draw_networkx(G)
plt.savefig("../dataset/test_dataset.png")
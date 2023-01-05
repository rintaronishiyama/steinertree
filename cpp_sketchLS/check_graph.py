import networkx as nx
import matplotlib.pyplot as plt

def draw_graph(graph_path) :
    G = nx.Graph()
    f = open(graph_path, 'r')
    lines = f.readlines()
    f.close()

    for line in lines :
        nodes = line.split()
        for i in range(2, len(nodes)) :
            G.add_edge( int(nodes[0]), int(nodes[i]))
    
    if len(nx.cycle_basis(G)) != 0 :
        print(graph_path + " has cycle")

    nx.draw_networkx(G)
    save_path = graph_path[:-3] + ".png"
    plt.savefig(save_path)
    plt.close()

draw_graph("test.txt")
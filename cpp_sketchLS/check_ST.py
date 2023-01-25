import os
import networkx as nx
import matplotlib.pyplot as plt

def draw_ST(graph_path) :
    G = nx.Graph()
    f = open(graph_path, 'r')
    lines = f.readlines()
    f.close()

    for line in lines :
        #　ターミナルの行を無視
        if line == lines[-1] :
            break

        nodes = line.split()
        for i in range(2, len(nodes)) :
            G.add_edge( int(nodes[0]), int(nodes[i]))

    # ターミナルを色付け
    terminal_list = [int(i) for i in lines[-1].split()]
    color_array = []
    for node in G :
        if terminal_list.count(node) != 0 :
            color_array.append("red")
        else :
            color_array.append("aqua")
    
    if len(nx.cycle_basis(G)) != 0 :
        print(graph_path + " has cycle")

    nx.draw_networkx(G, node_color=color_array)
    save_path = graph_path[:-3] + ".png"
    plt.savefig(save_path)
    plt.close()

def draw_ST_in_dir(dir_path) :
    files = os.listdir(dir_path)
    for file in files :
        if file[-3:] == "txt" :
            draw_ST(dir_path + "/" + file)


graph_name_list = ["ego-facebook", "large-facebook"]
interval_list   = ["add", "increment", "multiply"]
centrality_list = ["DC", "CC", "BC"]

for graph_name in graph_name_list :
    for interval in interval_list :
        for centrality in centrality_list :
            draw_ST_in_dir("./" + graph_name + "/" + interval + "/" + centrality + "/ST_checking")
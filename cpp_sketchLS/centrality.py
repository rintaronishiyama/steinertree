import networkx as nx
import os
import matplotlib.pyplot as plt
def read_centrality(data_path) :
    f = open(data_path, 'r')
    lines = f.readlines()
    f.close()

    dict = {}

    for line in lines :
        data = line.split()
        dict[int(data[0])] = float(data[1])

    return dict

def differentiate(y) :
    max = 0
    max_pos = 0
    for i in range(1,len(y) - 1) :
        prev = y[i] - y[i - 1]
        next = y[i + 1] - y[i]
        
        if prev == 0 :
            continue

        diff = next / prev
        if diff > max :
            max = diff
            max_pos = i

    return max_pos

        

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
x = list(range(n))
for i in range(len(x)) :
    x[i] /= n
    x[i] *= 100

""" centralityを読み込み """
BC_dict = read_centrality("./" + dataset_name + "/BC.txt")
CC_dict = read_centrality("./" + dataset_name + "/CC.txt")
DC_dict = read_centrality("./" + dataset_name + "/DC.txt")

""" 昇順にソート """
BC_dict = dict( sorted(BC_dict.items(), key = lambda BC : BC[1]) )
CC_dict = dict( sorted(CC_dict.items(), key = lambda CC : CC[1]) )
DC_dict = dict( sorted(DC_dict.items(), key = lambda DC : DC[1]) )

""" 中心性下位のノードから足し合わせる """
y_for_BC = []
y_for_CC = []
y_for_DC = []


for bc in BC_dict.values() :
    y_for_BC.append(float(bc))

for cc in CC_dict.values() :
    y_for_CC.append(float(cc))

for dc in DC_dict.values() :
    y_for_DC.append(float(dc))

""" 微分 """
print("bc diff max point : " + str(differentiate(y_for_BC)))
print("dc diff max point : " + str(differentiate(y_for_DC)))

""" 結果を保存するディレクトリ作成 """
result_dir_path = "./" + dataset_name + "/centrality_analysis"
os.makedirs(result_dir_path, exist_ok=True)

""" plot """
plt.plot(x, y_for_BC)
plt.grid()
plt.tick_params(labelsize=15)
plt.xlabel("nodes ratio[%]", fontsize=15)
plt.ylabel("BC", fontsize=15)
plt.tight_layout()
plt.savefig(result_dir_path + "/BC.pdf")
plt.close()

plt.plot(x, y_for_CC)
plt.grid()
plt.tick_params(labelsize=15)
plt.xlabel("nodes ratio[%]", fontsize=15)
plt.ylabel("CC", fontsize=15)
plt.tight_layout()
plt.savefig(result_dir_path + "/CC.pdf")
plt.close()

plt.plot(x, y_for_DC)
plt.grid()
plt.tick_params(labelsize=15)
plt.xlabel("nodes ratio[%]", fontsize=15)
plt.ylabel("DC", fontsize=15)
plt.tight_layout()
plt.savefig(result_dir_path + "/DC.pdf")
plt.close()


import matplotlib.pyplot as plt

graph_name = "large-facebook"
f = open("./" + graph_name + "/overlap_ratio.txt", 'r')
lines = f.readlines()
f.close()
x = []
y = []
for line in lines :
    data = line.split()
    x.append(data[0] + "~" + data[1] + "")
    y.append(float(data[3]))
plt.bar(x,y)
plt.xticks(rotation=45)
plt.xlabel("degree top nodes[%]")
plt.ylabel("overlap ratio[%]")
plt.savefig("./" + graph_name + "/overlap_ratio.png")
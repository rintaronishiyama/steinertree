import matplotlib.pyplot as plt
def plot_overlap_ratio(data_path) :
    f = open(data_path, 'r')
    lines = f.readlines()
    f.close()
    x = []
    y = []
    for line in lines :
        data = line.split()
        x.append(data[0] + "~" + data[1] + "")
        y.append(float(data[3]) * 100)
    plt.bar(x,y)
    plt.xticks(rotation=45)
    plt.xlabel("degree top nodes[%]")
    plt.ylabel("overlap ratio[%]")
    plt.savefig(data_path[:-3] + "png")
    plt.close()

def plot_size(data_path) :
    f = open(data_path, 'r')
    lines = f.readlines()
    f.close()
    x = []
    y = []
    for line in lines :
        data = line.split()
        if len(data) == 3 :
            x.append(data[0])
            y.append(float(data[2]))
            continue
        x.append(data[0] + "~" + data[1])
        y.append(float(data[3]))
    plt.bar(x,y)
    plt.xticks(rotation=45)
    plt.xlabel("degree top nodes[%]")
    plt.ylabel("size")
    plt.savefig(data_path[:-3] + "png")
    plt.close()

graph_name = "large-facebook"
plot_overlap_ratio("./" + graph_name + "/overlap_ratio.txt")
plot_overlap_ratio("./" + graph_name + "/overlap_ratio_avoid0.100000bc.txt")
plot_overlap_ratio("./" + graph_name + "/overlap_ratio_avoid0.050000bc.txt")
plot_overlap_ratio("./" + graph_name + "/overlap_ratio_avoid0.010000bc.txt")

plot_size("./" + graph_name + "/size_avoid0.100000bc.txt")
plot_size("./" + graph_name + "/size_avoid0.050000bc.txt")
plot_size("./" + graph_name + "/size_avoid0.010000bc.txt")
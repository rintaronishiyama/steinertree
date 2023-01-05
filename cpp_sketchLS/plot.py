import matplotlib.pyplot as plt

def plot_overlap_ratio(data_path) :
    f = open(data_path, 'r')
    lines = f.readlines()
    f.close()
    x = []
    y = []
    for line in lines :
        data = line.split()
        x.append(data[0])
        y.append(float(data[1]) * 100)
    plt.bar(x,y)
    plt.xticks(rotation=45)
    plt.xlabel("number of avoided bc top nodes")
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
        x.append(data[0])
        y.append(float(data[1]))
    plt.bar(x,y)
    plt.xticks(rotation=45)
    plt.xlabel("number of avoided bc top nodes")
    plt.ylabel("size")
    plt.savefig(data_path[:-3] + "png")
    plt.close()

graph_name = "large-facebook"
plot_overlap_ratio("./" + graph_name + "/overlap_ratio.txt")
plot_size("./" + graph_name + "/size.txt")
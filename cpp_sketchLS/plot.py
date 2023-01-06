import matplotlib.pyplot as plt

def plot_overlap_ratio(data_path) :
    f = open(data_path, 'r')
    lines = f.readlines()
    f.close()

    x = {}
    y = {}
    
    for line in lines :
        data = line.split()

        if not data[0] in x :
            x[data[0]] = []
            y[data[0]] = []

        x[data[0]].append(data[1])
        y[data[0]].append(float(data[2]) * 100)
        
    for key in x :
        plt.plot(x[key], y[key], label=key)
    
    plt.grid()
    plt.legend(loc=0, title="top bc nodes")
    
    plt.title(data_path[2:-4])
    plt.xticks(rotation=45)
    plt.xlabel("sketch range[%]")
    plt.ylabel("overlap ratio[%]")
    plt.savefig(data_path[:-3] + "png")
    plt.close()

def plot_size(data_path) :
    f = open(data_path, 'r')
    lines = f.readlines()
    f.close()

    x = {}
    y = {}
    
    for line in lines :
        data = line.split()

        if not data[0] in x :
            x[data[0]] = []
            y[data[0]] = []

        x[data[0]].append(data[1])
        y[data[0]].append(float(data[2]))
        
    for key in x :
        plt.plot(x[key], y[key], label=key)
    
    plt.grid()
    plt.legend(loc=0, title="top bc nodes")
    
    plt.title(data_path[2:-4])
    plt.xticks(rotation=45)
    plt.xlabel("sketch range[%]")
    plt.ylabel("size")
    plt.savefig(data_path[:-3] + "png")
    plt.close()

def plot_sum_of_degree(data_path) :
    f = open(data_path, 'r')
    lines = f.readlines()
    f.close()

    x = {}
    y = {}
    
    for line in lines :
        data = line.split()

        if not data[0] in x :
            x[data[0]] = []
            y[data[0]] = []

        x[data[0]].append(data[1])
        y[data[0]].append(float(data[2]))
        
    for key in x :
        plt.plot(x[key], y[key], label=key)
    
    plt.grid()
    plt.legend(loc=0, title="top bc nodes")
    
    plt.title(data_path[2:-4])
    plt.xticks(rotation=45)
    plt.xlabel("sketch range[%]")
    plt.ylabel("sum of degree")
    plt.savefig(data_path[:-3] + "png")
    plt.close()

def plot_sum_of_bc(data_path) :
    f = open(data_path, 'r')
    lines = f.readlines()
    f.close()

    x = {}
    y = {}
    
    for line in lines :
        data = line.split()

        if not data[0] in x :
            x[data[0]] = []
            y[data[0]] = []

        x[data[0]].append(data[1])
        y[data[0]].append(float(data[2]))
        
    for key in x :
        plt.plot(x[key], y[key], label=key)
    
    plt.grid()
    plt.legend(loc=0, title="top bc nodes")
    
    plt.title(data_path[2:-4])
    plt.xticks(rotation=45)
    plt.xlabel("sketch range[%]")
    plt.ylabel("sum of bc")
    plt.savefig(data_path[:-3] + "png")
    plt.close()

graph_name = "ego-facebook"
plot_overlap_ratio("./" + graph_name + "/overlap_ratio.txt")
plot_size("./" + graph_name + "/size.txt")
plot_sum_of_degree("./" + graph_name + "/sum_of_degree.txt")
plot_sum_of_bc("./" + graph_name + "/sum_of_bc.txt")

graph_name1 = "large-facebook"
plot_overlap_ratio("./" + graph_name1 + "/overlap_ratio.txt")
plot_size("./" + graph_name1 + "/size.txt")
plot_sum_of_degree("./" + graph_name1 + "/sum_of_degree.txt")
plot_sum_of_bc("./" + graph_name1 + "/sum_of_bc.txt")
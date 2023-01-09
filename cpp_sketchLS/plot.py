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


def plot_precomputation_time(data_path) :
    f = open(data_path, 'r')
    lines = f.readlines()
    f.close()

    x = []
    y_original = []
    y_proposal = []

    original = 0
    proposal = 0

    length = 50
    count = 0
    for line in lines :
        count += 1
        data = line.split()

        if count == 1 :
            bottom = data[0]

        original += float( data[4][:-2] )
        proposal += float( data[5][:-2] )

        if count == length :
            top = data[2]
            x.append(bottom + "~" + top)
            y_original.append(original)
            y_proposal.append(proposal)
            
            count = 0
            original = 0
            proposal = 0

    plt.bar(x, y_original, color='b', label="original", align="center")
    plt.bar(x, y_proposal, color='r', label="proposal", align="center")
    plt.title(data_path[2:-4])
    plt.xticks(rotation=45)
    plt.xlabel("sketch range[%]")
    plt.ylabel("precomputation time[ms]")
    plt.savefig(data_path[:-3] + "png")
    plt.close()

        

graph_name = "ego-facebook"
plot_overlap_ratio("./" + graph_name + "/degree/overlap_ratio.txt")
plot_overlap_ratio("./" + graph_name + "/bc/overlap_ratio.txt")
plot_size("./" + graph_name + "/degree/size.txt")
plot_size("./" + graph_name + "/bc/size.txt")
plot_sum_of_degree("./" + graph_name + "/degree/sum_of_degree.txt")
plot_sum_of_degree("./" + graph_name + "/bc/sum_of_degree.txt")
plot_sum_of_bc("./" + graph_name + "/degree/sum_of_bc.txt")
plot_sum_of_bc("./" + graph_name + "/bc/sum_of_bc.txt")
plot_precomputation_time("./" + graph_name + "/degree/precomputation.txt")
plot_precomputation_time("./" + graph_name + "/bc/precomputation.txt")

graph_name = "large-facebook"
plot_overlap_ratio("./" + graph_name + "/degree/overlap_ratio.txt")
plot_overlap_ratio("./" + graph_name + "/bc/overlap_ratio.txt")
plot_size("./" + graph_name + "/degree/size.txt")
plot_size("./" + graph_name + "/bc/size.txt")
plot_sum_of_degree("./" + graph_name + "/degree/sum_of_degree.txt")
plot_sum_of_degree("./" + graph_name + "/bc/sum_of_degree.txt")
plot_sum_of_bc("./" + graph_name + "/degree/sum_of_bc.txt")
plot_sum_of_bc("./" + graph_name + "/bc/sum_of_bc.txt")
plot_precomputation_time("./" + graph_name + "/degree/precomputation.txt")
plot_precomputation_time("./" + graph_name + "/bc/precomputation.txt")
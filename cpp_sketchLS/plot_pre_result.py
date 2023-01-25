import matplotlib.pyplot as plt
import os

def plot_avoidability_comparing_centrality(data_path, centrality_list) :

    # ノード数取得
    n = 22470

    x_list = []
    y_list = []
    label_list = []

    for centrality in centrality_list :
        avoidability_path = data_path + "/increment/" + centrality + "/avoidability.txt"
        f = open(avoidability_path, 'r')
        lines = f.readlines()
        f.close()

        x = []
        y = []

        for line in lines :
            data = line.split()

            x.append( ( int(data[0]) / n ) * 100)
            y.append( float(data[1]) * 100 )

        x_list.append(x)
        y_list.append(y)
        label_list.append(centrality)

    for i in range( len(x_list) ) :
        plt.plot(x_list[i], y_list[i], label=label_list[i])
    
    plt.grid()
    plt.legend(loc=0, title="centrality")
    plt.xlabel("top nodes ratio[%]")
    plt.ylabel("avoidability[%]")
    plt.savefig(data_path + "/avoidability.pdf")
    plt.close()


def plot_precomputation_time_comparing_centrality(data_path, interval_list, centrality_list) :
    f_for_seed = open(data_path + "/seed_node_sets_time.txt", 'r')
    lines_for_seed = f_for_seed.readlines()
    data_for_seed = lines_for_seed[0].split()
    seed_node_sets_time = float( data_for_seed[0] )

    # ノード数取得
    n = 22470

    for interval in interval_list :
        result_dir_path = data_path + "/" + interval

        x_list = []
        y_list = []
        label_list = []

        for centrality in centrality_list :
            precomputation_path = result_dir_path + "/" + centrality + "/precomputation.txt"
            f = open(precomputation_path, 'r')
            lines = f.readlines()
            f.close()
            
            x = []
            y = []

            total = 0
            for line in lines :
                data = line.split()

                total += float(data[1])

                x.append( (int(data[0]) / n ) * 100)
                y.append(total + seed_node_sets_time)
            
            x_list.append(x)
            y_list.append(y)
            label_list.append(centrality)

        for i in range( len(x_list) ) :
            plt.plot(x_list[i], y_list[i], label=label_list[i])
        
        plt.grid()
        plt.legend(loc=0, title="centrality")
        plt.xlabel("top nodes ratio[%]")
        plt.ylabel("precomputation time[ms]")
        plt.savefig(result_dir_path + "/precomputation.pdf")
        plt.close()

def plot_precomputation_time_comparing_interval(data_path, interval_list, centrality_list) :
    f_for_seed = open(data_path + "/seed_node_sets_time.txt", 'r')
    lines_for_seed = f_for_seed.readlines()
    data_for_seed = lines_for_seed[0].split()
    seed_node_sets_time = float( data_for_seed[0] )

    # ノード数取得
    n = 22470

    for centrality in centrality_list :

        x_list = []
        y_list = []
        label_list = []

        for interval in interval_list :
            precomputation_path = data_path + "/" + interval + "/" + centrality + "/precomputation.txt"
            f = open(precomputation_path, 'r')
            lines = f.readlines()
            f.close()
            
            x = []
            y = []

            total = 0
            for line in lines :
                data = line.split()

                total += float(data[1])

                x.append( (int(data[0]) / n ) * 100)
                y.append(total + seed_node_sets_time)
            
            x_list.append(x)
            y_list.append(y)
            label_list.append(interval)

        for i in range( len(x_list) ) :
            plt.plot(x_list[i], y_list[i], label=label_list[i])
        
        plt.grid()
        plt.legend(loc=0, title="interval")
        plt.xlabel("top nodes ratio[%]")
        plt.ylabel("precomputation time[ms]")
        plt.savefig(data_path + "/precomputation_" + centrality + ".pdf")
        plt.close()







graph_name_list = ["large-facebook"]
interval_list = ["increment", "add", "multiply"]
centrality_list = ["DC", "BC"]

for graph_name in graph_name_list :
    plot_avoidability_comparing_centrality("./" + graph_name, centrality_list)
    plot_precomputation_time_comparing_centrality("./" + graph_name, interval_list, centrality_list)
    plot_precomputation_time_comparing_interval("./" + graph_name, interval_list, centrality_list)
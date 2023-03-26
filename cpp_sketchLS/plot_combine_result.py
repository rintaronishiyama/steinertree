import matplotlib.pyplot as plt
import os

def get_seed_node_sets_time(data_path) :
    f_for_seed = open(data_path + "/seed_node_sets_time.txt", 'r')
    lines_for_seed = f_for_seed.readlines()
    data_for_seed = lines_for_seed[0].split()
    seed_node_sets_time = float( data_for_seed[0] )
    
    return seed_node_sets_time


def get_number_of_nodes(data_path) :
    f = open(data_path + "/node.txt", 'r')
    lines = f.readlines()
    return int( lines[0] )


def plot_STsize_sumofcentrality_comparing_interval(data_path, interval_list, centrality_list) :
    for centrality in centrality_list :
        x_list = []
        y1_list = []
        y2_list = []
        label_list1 = []
        label_list2 = []
        color_list = ["red", "darkorange", "forestgreen", "dodgerblue"]

        for interval in interval_list :
            result_dir_path = data_path + "/" + interval + "/" + centrality + "/terminals5"

            fx = open(result_dir_path + "/ST_size_ratio.txt", 'r')
            fy1 = open(result_dir_path + "/sum_of_DC_ratio.txt", 'r')
            fy2 = open(result_dir_path + "/sum_of_BC_ratio.txt", 'r')

            linesx = fx.readlines()
            linesy1 = fy1.readlines()
            linesy2 = fy2.readlines()

            fx.close()
            fy1.close()
            fy2.close()

            x = []
            y1 = []
            y2 = []

            for linex in linesx :
                data = linex.split()
                x.append(float(data[1]) * 100)
            
            for liney1 in linesy1 :
                data = liney1.split()
                y1.append(float(data[1]) * 100)

            for liney2 in linesy2 :
                data = liney2.split()
                y2.append(float(data[1]) * 100)

            x_list.append(x)
            y1_list.append(y1)
            y2_list.append(y2)

            if interval == "allornothing" :
                label_list1.append("DC/all or nothing")
                label_list2.append("BC/all or nothing")
            else :
                label_list1.append("DC/" + interval)
                label_list2.append("BC/" + interval)

        fig = plt.figure()
        
        ax1 = fig.subplots()
        ax2 = ax1.twinx()

        for i in range( len(x_list) ) :
            ax1.plot(x_list[i], y1_list[i], color=color_list[i], label=label_list1[i])
            ax2.plot(x_list[i], y2_list[i], color=color_list[i], label=label_list2[i], linestyle="dashed")
            

        h1, l1 = ax1.get_legend_handles_labels()
        h2, l2 = ax2.get_legend_handles_labels()
        ax1.legend(h1 + h2, l1 + l2, ncol=2, title="sum of/interval")

        ax1.tick_params(labelsize=15)
        ax2.tick_params(labelsize=15)

        ax1.set_xlabel("ST size ratio[%]", fontsize=15)

        ax1.set_ylabel("sum of DC ratio[%]", fontsize=15)
        ax2.set_ylabel("sum of BC ratio[%]", fontsize=15)

        fig.tight_layout()
        
        fig.savefig(data_path + "/" + centrality + "_STsize_sumofcentrality.pdf")
        plt.close()


def plot_STsize_precomputationtime_comparing_interval(data_path, interval_list, centrality_list) :
    seed_node_sets_time = get_seed_node_sets_time(data_path)

    x_list = []
    y_list = []
    label_list = []
    # color_list = ["red", "darkorange", "fsorestgreen", "dodgerblue", "red", "darkorange", "forestgreen", "dodgerblue"]
    color_list = ["darkorange", "forestgreen", "dodgerblue", "darkorange", "forestgreen", "dodgerblue"]

    for centrality in centrality_list :
        for interval in interval_list :
            result_data_path = data_path + "/" + interval + "/" + centrality

            fx = open(result_data_path + "/terminals5/ST_size_ratio.txt", 'r')
            fy = open(result_data_path + "/precomputation.txt", 'r')
            linesx = fx.readlines()
            linesy = fy.readlines()
            fx.close()
            fy.close()

            x = []
            y = []

            total = 0

            for linex in linesx :
                data = linex.split()
                x.append(float(data[1]) * 100)

            for liney in linesy :
                data = liney.split()
                total += float(data[1])
                y.append(total + seed_node_sets_time)

            x_list.append(x)
            y_list.append(y)
            if interval == "allornothing" :
                label_list.append(centrality + "/all or nothing")
            else :
                label_list.append(centrality + "/" + interval)

    for i in range( len(x_list) ) :
        if i < len(interval_list) :
            plt.plot(x_list[i], y_list[i], color=color_list[i], label=label_list[i])
        else :
            plt.plot(x_list[i], y_list[i], color=color_list[i], label=label_list[i], linestyle="dashed")

    plt.legend(title="centrality/interval", ncol=2, loc="upper left", fontsize=9)
    plt.tick_params(labelsize=15)
    plt.xlabel("ST size raito[%]", fontsize=15)
    plt.ylabel("precomputation time[ms]", fontsize=15)
    plt.grid()
    plt.tight_layout()
    plt.savefig(data_path + "/STsize_precomputationtime.pdf")
    plt.close()

def plot_STsize_sumofcentrality_comparing_terminals(data_path, interval_list, centrality_list, terminals_list) :
    for interval in interval_list :
        for centrality in centrality_list :
            x_list = []
            y1_list = []
            y2_list = []
            label_list1 = []
            label_list2 = []
            color_list = ["red", "darkorange", "forestgreen", "dodgerblue", "mediumorchid"]

            for terminals in terminals_list :
                result_dir_path = data_path + "/" + interval + "/" + centrality + "/" + terminals

                fx = open(result_dir_path + "/ST_size_ratio.txt", 'r')
                fy1 = open(result_dir_path + "/sum_of_DC_ratio.txt", 'r')
                fy2 = open(result_dir_path + "/sum_of_BC_ratio.txt", 'r')

                linesx = fx.readlines()
                linesy1 = fy1.readlines()
                linesy2 = fy2.readlines()

                fx.close()
                fy1.close()
                fy2.close()

                x = []
                y1 = []
                y2 = []

                for linex in linesx :
                    data = linex.split()
                    x.append(float(data[1]) * 100)
                
                for liney1 in linesy1 :
                    data = liney1.split()
                    y1.append(float(data[1]) * 100)

                for liney2 in linesy2 :
                    data = liney2.split()
                    y2.append(float(data[1]) * 100)

                x_list.append(x)
                y1_list.append(y1)
                y2_list.append(y2)

                label_list1.append("DC/" + terminals[-1:])
                label_list2.append("BC/" + terminals[-1:])
            
            fig = plt.figure()
        
            ax1 = fig.subplots()
            ax2 = ax1.twinx()

            for i in range( len(x_list) ) :
                ax1.plot(x_list[i], y1_list[i], color=color_list[i], label=label_list1[i])
                ax2.plot(x_list[i], y2_list[i], color=color_list[i], label=label_list2[i], linestyle="dashed")
                

            h1, l1 = ax1.get_legend_handles_labels()
            h2, l2 = ax2.get_legend_handles_labels()
            ax1.legend(h1 + h2, l1 + l2, ncol=2, title="sum of/number of terminals")

            ax1.tick_params(labelsize=15)
            ax2.tick_params(labelsize=15)

            ax1.set_xlabel("ST size ratio[%]", fontsize=15)

            ax1.set_ylabel("sum of DC ratio[%]", fontsize=15)
            ax2.set_ylabel("sum of BC ratio[%]", fontsize=15)

            fig.tight_layout()
            
            fig.savefig(data_path + "/" + interval + "/" + centrality + "/" + centrality + "_STsize_sumofcentrality.pdf")
            plt.close()

graph_name_list = ["large-facebook"]
# interval_list = ["increment", "add", "multiply", "allornothing"]
terminals_list = ["terminals3", "terminals4", "terminals5", "terminals6", "terminals7"]
interval_list = ["add", "multiply", "allornothing"]
centrality_list = ["DC", "BC"]
centrality_list2 = ["DC"]

for graph_name in graph_name_list :
    # plot_STsize_sumofcentrality_comparing_interval("./" + graph_name, interval_list, centrality_list)
    plot_STsize_precomputationtime_comparing_interval("./" + graph_name, interval_list, centrality_list)
    # plot_STsize_sumofcentrality_comparing_terminals("./" + graph_name, interval_list, centrality_list, terminals_list)
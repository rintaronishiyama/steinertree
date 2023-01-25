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


def plot_combine_evaluation_comparing_centrality(data_path, interval_list, centrality_list, evaluation_list) :
    seed_node_sets_time = get_seed_node_sets_time(data_path)

    # ノード数取得
    n = get_number_of_nodes(data_path)

    for interval in interval_list :
        result_dir_path = data_path + "/" + interval

        x_list = []
        y_list_list = [] # 外 evaluation 内 centrality
        label_list = []

        # 事前計算時間を取得
        for centrality in centrality_list :
            precomputation_path = result_dir_path + "/" + centrality + "/precomputation.txt"
            f = open(precomputation_path, 'r')
            lines = f.readlines()
            f.close()
            
            x = []

            total = 0
            for line in lines :
                data = line.split()

                total += float(data[1])

                x.append(total + seed_node_sets_time)
            
            x_list.append(x)

        # 評価の値を取得
        for evaluation in evaluation_list :
            y_list = []
            for centrality in centrality_list :
                # 事前計算時間を取得
                evaluation_path = result_dir_path + "/" + centrality + "/terminals5/" + evaluation + ".txt"
                f = open(evaluation_path, 'r')
                lines = f.readlines()
                f.close()
                
                y = []

                for line in lines :
                    data = line.split()
                    y.append( float(data[1]) )
                
                y_list.append(y)
            
            y_list_list.append(y_list)


        for centrality in centrality_list :
            label_list.append(centrality)

        for i in range(len(evaluation_list)) :

            for j in range( len(x_list) ) :
                plt.plot(x_list[j], y_list_list[i][j], label=label_list[j])
        
            plt.grid()
            plt.legend(loc=0, title="centrality")
            plt.xlabel("precomputation time[ms]")
            plt.ylabel( evaluation_list[i].replace('_', ' ') + "[%]")
            plt.savefig(result_dir_path + "/" + evaluation_list[i] + ".pdf")
            plt.close()

def plot_combine_evaluation_comparing_interval(data_path, interval_list, centrality_list, evaluation_list) :
    seed_node_sets_time = get_seed_node_sets_time(data_path)

    # ノード数取得
    n = get_number_of_nodes(data_path)

    for centrality in centrality_list :
        result_dir_path = data_path + "/" + centrality

        x_list = []
        y_list_list = [] # 外 evaluation 内 centrality
        label_list = []

        # 事前計算時間を取得
        for interval in interval_list :
            precomputation_path = data_path + "/" + interval + "/" + centrality + "/precomputation.txt"
            f = open(precomputation_path, 'r')
            lines = f.readlines()
            f.close()
            
            x = []

            total = 0
            for line in lines :
                data = line.split()

                total += float(data[1])

                x.append(total + seed_node_sets_time)
            
            x_list.append(x)

        # 評価の値を取得
        for evaluation in evaluation_list :
            y_list = []
            for interval in interval_list :
                # 事前計算時間を取得
                evaluation_path = data_path + "/" + interval + "/" + centrality + "/terminals5/" + evaluation + ".txt"
                f = open(evaluation_path, 'r')
                lines = f.readlines()
                f.close()
                
                y = []

                for line in lines :
                    data = line.split()
                    y.append( float(data[1]) )
                
                y_list.append(y)
            
            y_list_list.append(y_list)


        for interval in interval_list :
            label_list.append(interval)

        for i in range(len(evaluation_list)) :

            for j in range( len(x_list) ) :
                plt.plot(x_list[j], y_list_list[i][j], label=label_list[j])
        
            plt.grid()
            plt.legend(loc=0, title="interval")
            plt.xlabel("precomputation time[ms]")
            plt.ylabel( evaluation_list[i].replace('_', ' ') + "[%]")
            plt.savefig(result_dir_path + "/" + evaluation_list[i] + ".pdf")
            plt.close()






graph_name_list = ["large-facebook"]
interval_list = ["increment", "add", "multiply"]
evaluation_list = ["ST_size_ratio", "sum_of_BC_ratio", "sum_of_DC_ratio"]
centrality_list = ["DC", "BC"]

for graph_name in graph_name_list :
    plot_combine_evaluation_comparing_centrality("./" + graph_name, interval_list, centrality_list, evaluation_list)
    plot_combine_evaluation_comparing_interval("./" + graph_name, interval_list, centrality_list, evaluation_list)
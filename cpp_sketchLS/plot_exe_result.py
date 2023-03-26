import matplotlib.pyplot as plt
import os

def plot_evaluation_comparing_terminals(data_path, evaluation) :
    files = os.listdir(data_path)
    
    # ノード数取得
    n = 22470

    # [3, 4, 5, 6, 7]
    x_list = []
    y_list = []
    label_list = []

    # データ取得
    for i in range(3,8) :
        txt_path = data_path + "/terminals" + str(i) + "/" + evaluation + ".txt"
        f = open(txt_path, 'r')
        lines = f.readlines()
        f.close()

        x = []
        y = []

        for line in lines :
            data = line.split()

            x.append( (int(data[0]) / n) * 100 )
            y.append( float(data[1]) )
        
        x_list.append(x)
        y_list.append(y)
        label_list.append(i)
    
    # plot
    for i in range( len(x_list) ) :
        plt.plot(x_list[i], y_list[i], label=label_list[i])
    
    plt.grid()
    plt.legend(loc=0, title="number of terminals")
    plt.xlabel("top nodes ratio[%]")
    if "ratio" in evaluation :
        plt.ylabel( evaluation.replace('_', ' ') + "[%]")
    elif evaluation == "execution_time" :
        plt.ylabel( evaluation.replace('_', ' ') + "[ms]")
    else :
        plt.ylabel( evaluation.replace('_', ' ') )
    plt.savefig(data_path + "/" + evaluation + ".pdf")
    plt.close()


def plot_evaluation_comparing_centrality(data_path, evaluation, centrality_list, terminals) :
    result_dir_path = data_path + "/" + terminals
    os.makedirs(result_dir_path, exist_ok=True)
    
    # ノード数取得
    n = 22470

    # [dc, cc, bc]
    x_list = []
    y_list = []
    label_list = []

    # データ取得
    for centrality in centrality_list :
        file_path = data_path + "/" + centrality + "/" + terminals + "/" + evaluation + ".txt"
        f = open(file_path, 'r')
        lines = f.readlines()
        f.close()

        x = []
        y = []
        
        for line in lines :
            data = line.split()

            x.append( (int(data[0]) / n ) * 100)
            y.append( float(data[1]) )

        x_list.append(x)
        y_list.append(y)
        label_list.append(centrality)
    
    #plot
    for i in range( len(x_list) ) :
        plt.plot(x_list[i], y_list[i], label=label_list[i])
    
    plt.grid()
    plt.legend(loc=0, title="centrality")
    plt.xlabel("top nodes ratio[%]")
    if "ratio" in evaluation :
        plt.ylabel( evaluation.replace('_', ' ') + "[%]")
    elif evaluation == "execution_time" :
        plt.ylabel( evaluation.replace('_', ' ') + "[ms]")
    else :
        plt.ylabel( evaluation.replace('_', ' ') )
    plt.savefig(result_dir_path + "/" + evaluation + ".pdf")
    plt.close()


def plot_evaluation_comparing_interval(data_path, interval_list, evaluation, centrality, terminals_list) :
    result_dir_path = data_path + "/" + centrality
    os.makedirs(result_dir_path, exist_ok=True)

    # ノード数取得
    n = 22470

    for terminals in terminals_list :
        result_dir_path = data_path + "/" + centrality + "/" + terminals
        os.makedirs(result_dir_path, exist_ok=True)
    
        x_list = []
        y_list = []
        label_list = []

        # データ取得
        for interval in interval_list :
            file_path = data_path + "/" + interval + "/" + centrality + "/" + terminals + "/" + evaluation + ".txt"
            f = open(file_path, 'r')
            lines = f.readlines()
            f.close()

            x = []
            y = []
            
            for line in lines :
                data = line.split()

                x.append( (int(data[0]) / n ) * 100 )
                y.append( float(data[1]) )

            x_list.append(x)
            y_list.append(y)
            label_list.append(interval)
    
        #plot
        for i in range( len(x_list) ) :
            plt.plot(x_list[i], y_list[i], label=label_list[i])
        
        plt.grid()
        plt.legend(loc=0, title="interval")
        plt.xlabel("top nodes ratio[%]")
        if "ratio" in evaluation :
            plt.ylabel( evaluation.replace('_', ' ') + "[%]")
        elif evaluation == "execution_time" :
            plt.ylabel( evaluation.replace('_', ' ') + "[ms]")
        else :
            plt.ylabel( evaluation.replace('_', ' ') )
        plt.savefig(result_dir_path + "/" + evaluation + ".pdf")
        plt.close()


# graph_name_list = ["ego-facebook", "large-facebook"]
graph_name_list = ["large-facebook"]
interval_list   = ["increment", "add", "multiply"]
centrality_list = ["DC","BC"]
# centrality_list = ["DC", "CC", "BC"]
evaluation_list = ["ST_size", "ST_size_ratio", "sum_of_BC", "sum_of_DC", "sum_of_BC_ratio", "sum_of_CC_ratio", "sum_of_DC_ratio", "execution_time"]
# evaluation_list = ["overlap_ratio", "ST_size", "ST_size_ratio", "sum_of_BC", "sum_of_CC", "sum_of_DC", "sum_of_BC_ratio", "sum_of_CC_ratio", "sum_of_DC_ratio", "execution_time"]
terminals_list  = ["terminals3", "terminals4", "terminals5", "terminals6", "terminals7"]

for graph_name in graph_name_list :
    for interval in interval_list :
        for centrality in centrality_list :
            data_path = "./" + graph_name + "/" + interval + "/" + centrality
            for evaluation in evaluation_list :
                plot_evaluation_comparing_terminals(data_path, evaluation)

for graph_name in graph_name_list :
    for interval in interval_list :
        data_path = "./" + graph_name + "/" + interval
        for terminals in terminals_list :
            for evaluation in evaluation_list :
                plot_evaluation_comparing_centrality(data_path, evaluation, centrality_list, terminals)

for graph_name in graph_name_list :
    data_path = "./" + graph_name
    for centrality in centrality_list :
        for evaluation in evaluation_list :
            plot_evaluation_comparing_interval(data_path, interval_list, evaluation, centrality, terminals_list)
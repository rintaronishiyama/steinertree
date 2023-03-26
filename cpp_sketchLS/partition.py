def partition_graph(graph_path, partition_path):
    f = open(partition_path, 'r')
    lines = f.readlines()
    f.close()

    num_of_partition = 5

    # パーティションごとのノードのリスト
    node_list_partitioned = []

    for i in range(num_of_partition) :
        node_list_partitioned.append([])

    for line in lines :
        data = line.split()
        node = int(data[0])
        set = int(data[1])

        node_list_partitioned[set].append(node)

    f = open(graph_path, 'r')
    lines = f.readlines()
    f.close()

    # パーティションごとのエッジのリスト
    edge_list_partitioned = []

    for i in range(num_of_partition) :
        edge_list_partitioned.append([])


    for line in lines :
        data = line.split()

        for i in range(num_of_partition):
            if int(data[0]) in node_list_partitioned[i] :
                if int(data[1]) in node_list_partitioned[i]:
                    edge = []
                    edge.append(int(data[0]))
                    edge.append(int(data[1]))
                    edge_list_partitioned[i].append(edge)
    

    # パーティションごとにエッジを保存
    for i in range(num_of_partition) :
        f = open(graph_path[:-4] + str(i) + ".txt", 'w')
        for edge in edge_list_partitioned[i]:
            f.write(str(edge[0]) + " " + str(edge[1]) + "\n")


graph_path = "../dataset/ego-facebook.txt"
partition_path = "../dataset/partition.txt"

partition_graph(graph_path, partition_path)

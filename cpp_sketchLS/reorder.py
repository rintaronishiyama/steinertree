def reorder_id(graph_path) :
    f = open(graph_path, 'r')
    lines = f.readlines()
    lines2 = lines.copy()
    f.close()

    node_list = []

    for line in lines :
        edge = line.split()
        for node in edge:
            if not int(node) in node_list :
                node_list.append(int(node))

    reordering_dict = {}

    for i in range(len(node_list)) :
        reordering_dict[node_list[i]] = i

    f = open("../dataset/" + graph_path[:-4] + "_reordered.txt", 'w')
    for line2 in lines2 :
        edge2 = line2.split()
        f.write( str(reordering_dict[int(edge2[0])]) + " " + str(reordering_dict[int(edge2[1])]) + "\n")

    f.close()

graph_path = "../dataset/part-4.txt"

reorder_id(graph_path)
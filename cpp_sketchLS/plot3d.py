import matplotlib.pyplot as plt

def plot_3d_overlap_ratio(data_path) :
    f = open(data_path, 'r')
    lines = f.readlines()
    f.close()
    
    x = []
    y = []
    z = []

    x_tick_label = []
    y_tick_label = []

    countx = 0
    county = 0
    for line in lines :
        data = line.split()
        if countx == 0 :
            y_tick_label.append(data[1])

        x.append(countx)
        y.append(county)
        z.append( float(data[2]) * 100)

        county += 1
        if county == 20 :
            x_tick_label.append(data[0])

            county = 0
            countx += 1

    fig = plt.figure()
    ax = fig.add_subplot(projection='3d')

    ax.bar3d(x=x, y=y, z=0, dx=1, dy=1, dz=z)

    plt.xticks(range(len(x_tick_label)), x_tick_label)
    plt.yticks(range(len(y_tick_label)), y_tick_label)

    ax.set_xlabel("number of avoided bc top nodes")
    ax.set_ylabel("limit range")
    ax.set_zlabel("overlap ratio")
    plt.savefig(data_path[:-3] + "png")
    plt.close()


def plot_3d_size(data_path) :
    f = open(data_path, 'r')
    lines = f.readlines()
    f.close()
    
    x = []
    y = []
    z = []

    x_tick_label = []
    y_tick_label = []

    countx = 0
    county = 0
    for line in lines :
        data = line.split()
        if countx == 0 :
            y_tick_label.append(data[1])

        x.append(countx)
        y.append(county)
        z.append(float(data[2]))

        county += 1
        if county == 20 :
            x_tick_label.append(data[0])

            county = 0
            countx += 1

    fig = plt.figure()
    ax = fig.add_subplot(projection='3d')

    ax.bar3d(x=x, y=y, z=0, dx=1, dy=1, dz=z)

    plt.xticks(range(len(x_tick_label)), x_tick_label)
    plt.yticks(range(len(y_tick_label)), y_tick_label)

    ax.set_xlabel("number of avoided bc top nodes")
    ax.set_ylabel("limit range")
    ax.set_zlabel("overlap ratio")
    plt.savefig(data_path[:-3] + "png")
    plt.close()


graph_name = "ego-facebook"
plot_3d_overlap_ratio("./" + graph_name + "/overlap_ratio.txt")
plot_3d_size("./" + graph_name + "/size.txt")
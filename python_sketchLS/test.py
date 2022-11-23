import matplotlib.pyplot as plt
x = ["a", "b", "c"]

a = [1, 5, 4, 9]
b = [1, 3, 9]
c = [1, 2, 8]
nodelist = [a,b,c]
sum_node = a + b + c
sum_node.sort()
print(sum_node)
print(list(set(sum_node)))
sum_node = list(set(sum_node))
max_node = max(sum_node)
count = []
for i in range(len(sum_node)) :
    tmp = []
    for j in range(len(nodelist)) :
        if nodelist[j].count(sum_node[i]) != 0 :
            tmp.append(1)
        else :
            tmp.append(0)
    count.append(tmp.copy())
print(count)


bottomline = [0,0,0]

for i in range(len(count)) :
    if i == 0 :
        plt.bar(x, count[i], edgecolor="black", label=str(sum_node[i]), color=[sum_node[i]/max_node, 0, 1 - sum_node[i]/max_node])
    else :
        plt.bar(x, count[i], edgecolor="black",label=str(sum_node[i]), color=[sum_node[i]/max_node, 0, 1 - sum_node[i]/max_node], bottom=bottomline)
    for j in range(len(count[i])) :
        bottomline[j] += count[i][j]
plt.legend(loc=len(count))
plt.savefig("test.png")
plt.close()


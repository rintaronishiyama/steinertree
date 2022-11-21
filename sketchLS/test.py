import matplotlib.pyplot as plt
x = [1, 2, 3]
a = [1, 1, 0]
b = [1, 0, 1]
c = [1, 1, 1]
nodelist = [a,b,c]

color1 = [0,0,1]
color2 = [0,1,0]
color3 = [1,0,0]
colorlist = [color1, color2, color3]

bottomline = [0,0,0]

for i in range(len(colorlist)) :
    if i == 0 :
        plt.bar(x, nodelist[i], width=0.3, label=str(nodelist[i]), align="center")
        for j in range(len(nodelist[i])) :
            bottomline[j] += nodelist[i][j]
    else :
        plt.bar(x, nodelist[i], width=0.3, label=str(nodelist[i]), align="center", bottom=bottomline)
        for j in range(len(nodelist[i])) :
            bottomline[j] += nodelist[i][j]
# 凡例
plt.legend(loc=3)
plt.savefig("test.png")
plt.close()

# ファイル名入力
file_name = input("Enter the file to convert :")

# 読み込み
f = open(file_name, 'r')
lines = f.readlines()
f.close()

# 上書き
f = open(file_name, 'w')
for line in lines :
    data = line.split(',')
    f.write(data[0] + " " + data[1])
f.close()
    
import sys

dic = {}
#open Big5-ZhuYin.map
with open(sys.argv[1], "r", encoding='big5-hkscs') as inFile:
    for line in inFile:
        Big5 = line[0]
        ZhuYin = line.split()[1].split('/')
        for i in ZhuYin:
            key = i[0]
            if key not in dic:
                dic[key] = []
            if Big5 not in dic[key]:
                dic[key].append(Big5)

check = {}  #check polyphones to prvent redefine entry
#write ZhuYin-Big5.map
with open(sys.argv[2], 'w', encoding='big5-hkscs') as outFile:
    for key, value in dic.items():
        outFile.write(key + '\t' + ' '.join(value))
        outFile.write('\n')
        for v in value:
            if v not in check:
                outFile.write(v + '\t' + v)
                outFile.write('\n')
            check[v] = v

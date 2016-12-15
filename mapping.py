

mapping = dict()

with open('Big5-ZhuYin.map') as fp:
    for line in fp:
        character, zhuyin = line.split(' ')
        mapping[character] = [character]
        zhuyins = zhuyin.split('/')
        for z in zhuyins:
            if z[0:2] not in mapping:
                mapping[z[0:2]] = [character]
            else:
                mapping[z[0:2]].append(character)


for key, value in mapping.items():
    print key + '      ' + ' '.join(value) 
            
                

            







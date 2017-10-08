import shutil
import time

import sys

import os

def devide():
    with open(os.path.join('data','ideal','all.txt'),'r') as f:
        l = f.readlines()
    print(l)
    lines = []
    f_name = None
    for line in l:
        if '.JPG;' in line:
            if f_name:
                lines[0] = lines[0].replace('\n', str(len(lines)-1)+'\n')
                with open(os.path.join('data', 'ideal', f_name.replace('JPG', 'csv')), 'w') as f:
                    f.writelines(lines)
            lines = [line]
            f_name = line.split(';')[0]
        elif '.JPG_' in line:
            lines.append(line[9:])
        else:
            lines.append(line)
    lines[0] = lines[0].replace('\n', str(len(lines) - 1) + '\n')
    with open(os.path.join('data', 'ideal', f_name.replace('JPG', 'csv')), 'w') as f:
        f.writelines(lines)

def devide2():
    with open(os.path.join('data','ideal_crop','all.txt'),'r') as f:
        l = f.readlines()
    print(l)
    lines = []
    for i, line in enumerate(l):
        f_name = str(i) + line.split(';')[0].split('JPG')[1] + '.JPG'
        with open(os.path.join('data', 'ideal_crop', f_name.replace('JPG', 'csv')), 'w') as f:
            f.write(f_name.replace('JPG', 'png')+';0\n')
            f.write(';'.join(line.split(';')[1:]))

if __name__ == '__main__':
    devide2()
    if len(sys.argv) > 1:
        #time.sleep(0.1)
        print(sys.argv[1].replace('.JPG', '.csv'))
        shutil.copy(os.path.join('data', 'ideal', os.path.basename(sys.argv[1].replace('JPG', 'csv'))),
                    os.path.basename(sys.argv[1].replace('.JPG', '.csv')))


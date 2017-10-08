
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

if __name__ == '__main__':
    devide()
    if len(sys.argv) > 1:
        time.sleep(0.5)
        print(sys.argv[1].replace('.JPG', '.csv'))
        with open(os.path.basename(sys.argv[1].replace('.JPG', '.csv')), 'w') as f:
            f.write('zzz')

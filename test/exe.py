
import time

import sys

import os

if __name__ == '__main__':
    if len(sys.argv) > 1:
        time.sleep(0.5)
        print(sys.argv[1].replace('.JPG', '.csv'))
        with open(os.path.basename(sys.argv[1].replace('.JPG', '.csv')), 'w') as f:
            f.write('zzz')

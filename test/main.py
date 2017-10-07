import datetime
import unittest
import configparser
import subprocess
import time

import os

import numpy

typeimg = '.JPG'
typeres = '.csv'


class Result(object):

    def __init__(self, base_path):
        self.in_files = []
        self.times = []
        for f in os.listdir(base_path):
            if f.endswith(typeimg):
                self.in_files.append(os.path.join(base_path, f))

        self.res_files = list(map(lambda x: os.path.basename(x.replace(typeimg, typeres)), self.in_files))



class Metrics(object):
    pass


class Wrapper(object):

    def __init__(self, config_path):
        conf = configparser.ConfigParser()
        conf.add_section('DEF')
        conf.set('DEF', 'exe', 'python exe.py')
        conf.set('DEF', 'ideal', os.path.join('data', 'ideal'))
        conf.set('DEF', 'base', os.path.join('data', 'base'))
        #conf.set('DEF', 'res', os.path.join('out', 'result.csv'))

        with open(config_path, 'w') as conf_f:
            conf.write(conf_f)

        with open(config_path, 'r') as conf_f:
            conf.read(conf_f)

        self.exe_path = conf.get('DEF', 'exe')
        self.res_path = r'C:\PROJECTS\hackCV\shinnik\test' #os.path.dirname(self.exe_path)
        self.base = conf.get('DEF', 'base')


    def run(self):
        print('clean...')
        for f in os.listdir(self.res_path):
            if f.endswith(typeres):
                os.remove(os.path.join(self.res_path, f))
                print('del: ' + os.path.join(self.res_path, f))

        res = Result(self.base)

        for f in res.in_files:
            t1 = datetime.datetime.now()
            proc = subprocess.call(self.exe_path + ' ' + f)
            t_delta = datetime.datetime.now() - t1
            res.times.append(t_delta.total_seconds())

        return res


class SmokeTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        wrap = Wrapper('config.ini')
        cls.result = wrap.run()
        pass

    def test_something(self):
        for f in self.result.res_files:
            self.assertTrue(os.path.exists(f))

    def test_speed_max(self):
        self.assertLessEqual(max(self.result.times), 0.7)

    def test_speed_mean(self):
        self.assertLessEqual(numpy.mean(self.result.times), 0.5)

    def test_perfomance(self):
        pass


if __name__ == '__main__':
    unittest.main()

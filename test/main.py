import datetime
import unittest
import configparser
import subprocess
import time

import os

import numpy

typeimg = '.png'
typeres = '.csv'


class Result(object):

    def __init__(self, base_path, seg_path):
        self.in_files = []
        self.seg_files = []
        self.times = []
        self.true_class = []
        self.true_with_temp = []
        self.true_with_info = []
        self.false_class = []
        self.false_with_temp = []
        self.false_with_info = []
        for f in os.listdir(base_path):
            if f.endswith(typeimg):
                self.in_files.append(os.path.join(base_path, f))
                self.seg_files.append(os.path.join(seg_path, f.replace(typeimg, typeres)))

        self.res_files = list(map(lambda x: os.path.basename(x.replace(typeimg, typeres)), self.in_files))

    def get_total_signs(self):
        total = 0
        for f_name in self.seg_files:
            with open(f_name, 'r') as f:
                line = f.readline()
                total += int(line.strip().split(';')[1])
        return total

    def get_total_temp_signs(self):
        total = 0
        for f_name in self.seg_files:
            with open(f_name, 'r') as f:
                lines = f.readlines()
                for l in lines[1:]:
                    total += int(l.split(';')[1])
        return total

    def get_total_addinfo_signs(self):
        total = 0
        for f_name in self.seg_files:
            print(f_name)
            with open(f_name, 'r') as f:
                lines = f.readlines()
                for l in lines[1:]:
                    total += int(l.split(';')[0])
        return total

    def find(self, test_metric, ideal_seg_path):
        with open(ideal_seg_path, 'r') as f:
            ideals = f.readlines()[1:]
        res = None
        for ideal in ideals:
            m_i = Metric(ideal)
            if test_metric == m_i:
                res = m_i
                if m_i.info == test_metric.info:
                    return m_i
        return res

    def get_metrics(self):
        for x, v in enumerate(self.res_files):
            if os.path.exists(v):
                with open(v, 'r') as f:
                    for l in f.readlines()[1:]:
                        m = Metric(l)
                        id = self.find(m, self.seg_files[x])
                        name = v + ';' + l
                        if id:
                            self.true_class.append(name)
                            if m.temp == id.temp:
                                self.true_with_temp.append(name)
                            else:
                                self.false_with_temp.append(name)

                            if m.info == id.info:
                                self.true_with_info.append(name)
                            else:
                                self.false_with_info.append(name)

                        else:
                            self.false_class.append(name)
            else:
                print('NOT EXIST!!!: ' + v)







class Metric(object):

    def __init__(self, line):
        l = line.strip().split(';')
        self.type = l[2]
        self.temp = l[1]
        self.info = l[7]
        self.delta = int(l[5]) - int(l[3])
        self.x = int(l[3])
        self.y = int(l[4])

    def __eq__(self, other):
        if self.type == other.type and self.x-other.x < self.delta and self.y-other.y < self.delta:
            return True
        else:
            return False




class Wrapper(object):

    def __init__(self, config_path):
        conf = configparser.ConfigParser()
        conf.add_section('DEF')

        conf.set('DEF', 'exe', r'..\classificator_test_exe\build\Release\Shinik.exe')         #'python exe.py'
        conf.set('DEF', 'ideal', os.path.join('data', 'ideal_crop'))
        conf.set('DEF', 'base', os.path.join('data', 'base_crop'))
        #conf.set('DEF', 'res', os.path.join('out', 'result.csv'))

        with open(config_path, 'w') as conf_f:
            conf.write(conf_f)

        with open(config_path, 'r') as conf_f:
            conf.read(conf_f)

        self.exe_path = conf.get('DEF', 'exe')
        self.res_path = os.path.dirname(self.exe_path) #r'C:\PROJECTS\hackCV\shinnik\test' #
        self.base = conf.get('DEF', 'base')
        self.ideal = conf.get('DEF', 'ideal')


    def run(self):
        print('clean...')
        for f in os.listdir(self.res_path):
            if f.endswith(typeres):
                os.remove(os.path.join(self.res_path, f))
                print('del: ' + os.path.join(self.res_path, f))

        res = Result(self.base, self.ideal)

        for f in res.in_files:
            t1 = datetime.datetime.now()
            print(self.exe_path + ' ' + f)
            proc = subprocess.call(self.exe_path + ' ' + f)
            t_delta = datetime.datetime.now() - t1
            res.times.append(t_delta.total_seconds())

        res.get_metrics()
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

    def test_data(self):
        self.assertEqual(self.result.get_total_signs(), 236)
        self.assertEqual(self.result.get_total_temp_signs(), 8)
        self.assertEqual(self.result.get_total_addinfo_signs(), 44)

    def test_class_perfomance(self):
        recall = len(self.result.true_class)*1.0/self.result.get_total_signs()
        precision = len(self.result.true_class) * 1.0 / (len(self.result.true_class) + len(self.result.false_class))
        print(self.result.false_class)
        print('CLASS PRECISION: ' + str(precision))
        print('CLASS RECALL: ' + str(recall))
        self.assertEqual(precision, 1.0)
        self.assertEqual(recall, 1.0)

    def test_class_temp_perfomance(self):
        recall = len(self.result.true_with_temp)*1.0/self.result.get_total_signs()
        precision = len(self.result.true_with_temp) * 1.0 / (len(self.result.true_with_temp) + len(self.result.false_with_temp))
        print(self.result.false_with_temp)
        print('CLASS TEMP PRECISION: ' + str(precision))
        print('CLASS TEMP RECALL: ' + str(recall))
        self.assertEqual(precision, 1.0)
        self.assertEqual(recall, 1.0)

    def test_class_info_perfomance(self):
        recall = len(self.result.true_with_info)*1.0/self.result.get_total_signs()
        precision = len(self.result.true_with_info) * 1.0 / (len(self.result.true_with_info) + len(self.result.false_with_info))
        print(self.result.false_with_info)
        print('CLASS INFO PRECISION: ' + str(precision))
        print('CLASS INFO RECALL: ' + str(recall))
        self.assertEqual(precision, 1.0)
        self.assertEqual(recall, 1.0)



if __name__ == '__main__':
    unittest.main()

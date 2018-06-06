# -*- coding: utf-8 -*-

# Automatically outputs "sample_input.txt"
#
# Modified By: Kimisecond
#
# Directory should be like this:
# /homework
# /seq
# /random_test.py
#

import sys
import random
import os
import filecmp
import time

for i in xrange(1, 11):
    try:
        scale = random.randint(1, 100)
    except Exception, e:
        sys.stderr.write("error parsing array scale ...\n")
        raise
    f = open("sample_input.txt", "w")
    localtime = time.asctime(time.localtime(time.time()))
    print('File creation'), localtime
    # print('size'), dim_x, ('x'), dim_y
    print >> f, " ".join([str(random.randint(0, 9999)) for x in xrange(0, scale * 8)]) + " "
    f.close()
    localtime = time.asctime(time.localtime(time.time()))
    print('Sort Start'), localtime
    os.system("mpiexec -n 8 ./homework sample_input.txt 1>/dev/null")
    os.system("./seq sample_input.txt 1>/dev/null")
    localtime = time.asctime(time.localtime(time.time()))
    if filecmp.cmp('./output.txt', './output1.txt'):
        print('Success!'), (i), ('out of 10'), localtime
    else:
        print('Failed!'), (i), ('out of 10'), localtime
        exit(1)

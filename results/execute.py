#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import csv
import subprocess


with open('results/dataset.csv') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')
    line_count = 0
    for row in csv_reader:
        print("running project %s" % row[0])
        output_trasactions = "%s/results/go/%s_transactions.json" % (os.getcwd(), row[0])
        output_rules = "%s/results/go/%s_rules.csv" % (os.getcwd(), row[0])
        project_path = "%s/projects/%s" % (os.getcwd(), row[0])

        cmd = ['./main.py', '-l', 'go', '-d', project_path, '-s', '0.01',  '-c', '0.01', '-L',  '2', '-csv', output_rules,
               '-ot', output_trasactions]
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        o, err = proc.communicate()
        if err:
            print("ERROR", err)


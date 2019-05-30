#!/usr/bin/env python
# -*- coding: utf-8 -*-

import git
import os
import csv

with open('fetch/dataset.csv') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')
    line_count = 0
    for row in csv_reader:
        git_url = "https://github.com/%s.git" % row[0]
        print(git_url)

        gitpy = git.Git("%s/projects" % os.getcwd())
        print("Cloning repository to %s..." % row[0])
        gitpy.clone(git_url)

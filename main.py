#!/usr/bin/env python
# -*- coding: utf-8 -*-

import git
import argparse
from pydriller import RepositoryMining
from settings import CLONE_PATH
import os, hashlib

parser = argparse.ArgumentParser(description="Prepare something code.")
parser.add_argument("-p", "--project", help="Git project URL", required=True)
parser.add_argument("-s", "--support", help="correlation support value", default=0.8)


def generate_hash(size=8):
    return hashlib.md5(os.urandom(32)).hexdigest()[:size]


def main():
    project_path = "clone-%s" % generate_hash()
    args = parser.parse_args()
    gitpy = git.Git(CLONE_PATH)

    print("Cloning repository...")
    gitpy.clone(args.project, project_path)

    for commit in RepositoryMining("%s/%s" % (CLONE_PATH, project_path)).traverse_commits():
        print("COMMIT = ", commit.hash)
        # for mod in commit.modifications:
        #     print("DIFF=", mod.diff)
        #     print('{} has complexity of {}, and it contains {} methods'.format(mod.filename, mod.complexity, len(mod.methods)))
        #     for method in mod.methods:
        #         print(method.filename, method.start_line, method.end_line)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print("process interrupted")

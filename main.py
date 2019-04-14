#!/usr/bin/env python
# -*- coding: utf-8 -*-

import git
import os
import re
import hashlib
import argparse
from pydriller import RepositoryMining
from settings import CLONE_PATH

parser = argparse.ArgumentParser(description="Prepare something code.")
parser.add_argument("-p", "--project", help="Git project URL", required=True)
parser.add_argument("-s", "--support", help="correlation support value", default=0.8)

regexp_func_golang = re.compile("@.*\s(\S*.go).*(func.*{)")


def generate_hash(size=8):
    return hashlib.md5(os.urandom(32)).hexdigest()[:size]


def find_functions_change(line):
    match = regexp_func_golang.match(line)
    if match:
        return match.groups()


def main():
    project_folder = "clone-%s" % generate_hash()
    project_path = "%s/%s" % (CLONE_PATH, project_folder)
    args = parser.parse_args()
    gitpy = git.Git(CLONE_PATH)

    print("Cloning repository to %s..." % project_path)
    gitpy.clone(args.project, project_folder)

    for commit in RepositoryMining(project_path).traverse_commits():
        print("COMMIT = ", commit.hash)
        stdout = os.popen("cd %s && git show -U0 %s | diff-so-fancy | grep @" % (project_path, commit.hash)).read()
        for line in stdout.splitlines():
            matches = find_functions_change(line)
            if matches and len(matches) > 1:
                file = matches[0]
                function = matches[1]
                print("[%s] - %s" % (file, function))


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print("process interrupted")

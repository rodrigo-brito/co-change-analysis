#!/usr/bin/env python
# -*- coding: utf-8 -*-

import git
import argparse

from pydriller import RepositoryMining

from apriori import Apriori
from parsers.base import BaseParser
from parsers.golang import GoParser
from parsers.java import JavaParser
from parsers.python import PythonParser
from settings import CLONE_PATH
from utils.utils import generate_hash
from utils.utils import get_commit_count

p = argparse.ArgumentParser(description="Prepare something code.")
p.add_argument("-r", "--repository", help="Remote Git repository", required=False)
p.add_argument("-d", "--dir", help="Local project directory", required=False)
p.add_argument("-l", "--lang", help="Programming language", choices=['python', 'java', 'go'], required=True)
p.add_argument("-s", "--support", help="correlation support value", default=0.5)
p.add_argument("-c", "--confidence", help="correlation support value", default=0.5)
p.add_argument("-L", "--max_length", help="max number of items in a rule", default=2)

def main():
    args = p.parse_args()
    confidence = float(args.confidence)
    support = float(args.support)
    max_length = int(args.max_length)
    print("Apriori (support=%.3f, confidence=%.3f, max_length=%d)" % (support, confidence, max_length))
    
    gitpy = git.Git(CLONE_PATH)

    # fetching repository / folder
    project_path = args.dir
    if args.repository:
        project_folder = "clone-%s" % generate_hash()
        project_path = "%s/%s" % (CLONE_PATH, project_folder)
        print("Cloning repository to %s..." % project_path)
        gitpy.clone(args.repository, project_folder)

    print("number of commits: %d" % (get_commit_count(project_path)))
    print("support absolute value: %d" % (support * get_commit_count(project_path)))
    
    # defining language parser
    parser = BaseParser
    if args.lang == "go":
        parser = GoParser
    elif args.lang == "java":
        parser = JavaParser
    elif args.lang == "python":
        parser = PythonParser
    
    print("parsing project...")
    
    function_changes = []
    for commit in RepositoryMining(project_path).traverse_commits():
        language_parser = parser(project_path, commit.hash)
        changes = language_parser.get_diff()
        if changes:
            function_changes.append(changes)
            
    print("Transactions:")
    for changes in function_changes:
        print(changes)
    
    print("analyzing transactions...")
    
    apriori = Apriori(function_changes, confidence=float(confidence), support=float(support), max_length=int(max_length))
    rules = apriori.get_rules()
    
    print("Association rules:")
    for rule in rules:
        print(rule)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print("finished by user :)")

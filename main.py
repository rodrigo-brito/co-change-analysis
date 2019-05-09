#!/usr/bin/env python
# -*- coding: utf-8 -*-

import git
import argparse
import json

from pydriller import RepositoryMining

from apriori import Apriori
from parsers.base import BaseParser
from parsers.golang import GoParser
from parsers.java import JavaParser
from parsers.python import PythonParser
from settings import CLONE_PATH
from utils.utils import *
from pprint import pprint

p = argparse.ArgumentParser(description="Prepare something code.")
p.add_argument("-r", "--repository", help="Remote Git repository", required=False)
p.add_argument("-d", "--dir", help="Local project directory", required=False)
p.add_argument("-l", "--lang", help="Programming language", choices=['python', 'java', 'go'])
p.add_argument("-s", "--support", help="correlation support value", default=0.5)
p.add_argument("-c", "--confidence", help="correlation support value", default=0.5)
p.add_argument("-L", "--max_length", help="max number of items in a rule", default=2)
p.add_argument("-t", "--transactions", help="transactions file", required=False)
p.add_argument("-ot", "--out_transactions", help="write transactions file", required=False)

def main():
    args = p.parse_args()
    confidence = float(args.confidence)
    support = float(args.support)
    max_length = int(args.max_length)
    transactions_file_name = args.transactions
    
    print("support=%.3f" % (support))
    print("confidence=%.3f" % (confidence))
    print("max_length=%d" % (max_length))
    
    transactions = []
    
    if transactions_file_name is None:
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
        
        print("fetching transactions...")
        
        for commit in RepositoryMining(project_path).traverse_commits():
            language_parser = parser(project_path, commit.hash)
            items = language_parser.get_diff()
            if items:
                transactions.append(items)
    else:
        transaction_json = open(transactions_file_name, 'r').read()
        transactions = json.loads(transaction_json)
        
    print(ansi_color_yellow("Transactions:"))
    for i, changes in enumerate(transactions):
        print("%3d: " % (i), end='')
        print(changes)
        
    if args.out_transactions:
        out_file = open(args.out_transactions, 'w')
        json.dump(transactions, out_file, indent=2)
    
    print("analyzing transactions...")
    
    apriori = Apriori(transactions, confidence=float(confidence), support=float(support), max_length=int(max_length))
    rules = apriori.get_rules()
    
    print(ansi_color_yellow("Association rules:"))
    for i, rule in enumerate(rules):
        print("%3d: " % (i), end='')
        print(rule)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print("finished by user :)")

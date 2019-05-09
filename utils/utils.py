import hashlib
import os
from pydriller import RepositoryMining


def generate_hash(size=8):
    return hashlib.md5(os.urandom(32)).hexdigest()[:size]

def get_commit_count(project_path):
    result = 0
    for commit in RepositoryMining(project_path).traverse_commits():
        result += 1
    return result

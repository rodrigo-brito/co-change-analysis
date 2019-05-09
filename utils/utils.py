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

def ansi_color_red(text):
    return "\x1b[31m" + text + "\x1b[0m"

def ansi_color_green(text):
    return "\x1b[32m" + text + "\x1b[0m"

def ansi_color_yellow(text):
    return "\x1b[33m" + text + "\x1b[0m"

def ansi_color_blue(text):
    return "\x1b[34m" + text + "\x1b[0m"

def ansi_color_magenta(text):
    return "\x1b[35m" + text + "\x1b[0m"

def ansi_color_cyan(text):
    return "\x1b[36m" + text + "\x1b[0m"

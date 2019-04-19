import hashlib
import os


def generate_hash(size=8):
    return hashlib.md5(os.urandom(32)).hexdigest()[:size]

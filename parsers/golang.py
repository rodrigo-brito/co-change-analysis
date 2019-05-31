# -*- coding: utf-8 -*-

import re
from parsers.base import BaseParser


class GoParser(BaseParser):
    def is_invalid_file(self, file_name):
        if not file_name:
            return True

        return file_name.startswith("vendor/") or \
               file_name.endswith("_test.go") or \
               file_name.startswith("third_party/") or \
               not file_name.endswith(".go")

    def get_changes(self, diff):
        return set(re.findall(r"func (?:.* )?(\w+)\(.*\) \S*\s*{", diff, re.MULTILINE))

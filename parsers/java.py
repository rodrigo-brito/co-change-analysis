# -*- coding: utf-8 -*-
import re
from parsers.base import BaseParser


class JavaParser(BaseParser):
    def is_invalid_file(self, file_name):
        return False

    def get_changes(self, diff):
        return set(re.findall(r"(?:(?:public)|(?:private)|(?:static)|(?:protected)\s+)* \w+ +\w+ *\([^\)]*\) *\{", diff, re.MULTILINE))


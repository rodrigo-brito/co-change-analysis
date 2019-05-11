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
        matches = re.findall(
            r"func(?: \((?:.*\s)?(\w+)\))?\s+(\w+)\(.*\)\s*\{(?:[^func])*(?:\n|\r|\r\n|\n\r)(?:\+|\-)(?:[^func])*",
            diff, re.MULTILINE)

        functions = []
        if matches:
            for match in matches:
                if match[0]:
                    # struct with function
                    functions.append("%s.%s" % (match[0], match[1]))
                else:
                    # single function
                    functions.append(match[1])
        return functions

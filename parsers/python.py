import re

from parsers.base import BaseParser


class PythonParser(BaseParser):
    def get_changes(self, diff):
        matches = re.findall(
            r"(public|static|void|main)\(.*\)\{", # TODO: Python Parser
            diff, re.MULTILINE)

        functions = []
        if matches:
            pass
        return functions

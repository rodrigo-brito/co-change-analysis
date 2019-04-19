import re

from parsers.base import BaseParser


class JavaParser(BaseParser):
    def get_changes(self, diff):
        matches = re.findall(
            r"(public|static|void|main)\(.*\)\{", # TODO: Java Parser
            diff, re.MULTILINE)

        functions = []
        if matches:
            pass
        return functions

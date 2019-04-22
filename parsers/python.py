# -*- coding: utf-8 -*-

from parsers.base import BaseParser


class PythonParser(BaseParser):
    def is_invalid_file(self, file_name):
        return False

    def get_changes(self, diff):
        return []  # TODO: create python parser

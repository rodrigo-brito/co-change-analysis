# -*- coding: utf-8 -*-

from parsers.base import BaseParser


class JavaParser(BaseParser):
    def is_invalid_file(self, file_name):
        return False

    def get_changes(self, diff):
        return []  # TODO: create java parser

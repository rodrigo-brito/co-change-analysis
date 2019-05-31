# -*- coding: utf-8 -*-

import os
import re
import subprocess


class BaseParser:
    def __init__(self, project_path, commit_hash):
        self.project_path = project_path
        self.commit_hash = commit_hash

    def is_invalid_file(self, file_name):
        return False

    def get_diff(self):
        args = ["git", "show", "-U0", self.commit_hash]
        diff_proccess = subprocess.Popen(args, stdout=subprocess.PIPE, encoding="utf-8", errors="ignore",
                                         universal_newlines=True, stderr=subprocess.PIPE, cwd=self.project_path)

        out, err = diff_proccess.communicate()
        errcode = diff_proccess.returncode

        if errcode != 0:
            raise RuntimeError(err)

        blocks = out.split("diff --git")
        diffs = []
        for block in blocks:
            file_name = self.get_file_name(block)
            functions_changed = self.get_changes(block)

            if self.is_invalid_file(file_name):
                continue

            if file_name and functions_changed:
                for function in functions_changed:
                    diffs.append("%s|%s" % (file_name, function))

        diffs.sort()

        return diffs

    def get_file_name(self, block):
        first_line = block.splitlines()[0]
        match = re.findall(r"a\/([^\s]*)", first_line)
        if match:
            return match[0]

    def get_changes(self, diff):
        return []

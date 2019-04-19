import os
import re


class BaseParser:
    def __init__(self, project_path, commit_hash):
        self.project_path = project_path
        self.commit_hash = commit_hash

    def get_diff(self):
        diff_by_commit = os.popen("cd %s && git show --function-context %s" % (self.project_path,
                                                                               self.commit_hash)).read()
        blocks = diff_by_commit.split("diff --git")
        diffs = []
        for block in blocks:
            file_name = self.get_file_name(block)
            functions_changed = self.get_changes(block)

            if file_name and functions_changed:
                diffs.append({
                    'file': file_name,
                    'changes': functions_changed,
                })
        return diffs

    def get_file_name(self, block):
        first_line = block.splitlines()[0]
        match = re.findall(r"a\/([^\s]*)", first_line)
        if match:
            return match[0]

    def get_changes(self, diff):
        return []

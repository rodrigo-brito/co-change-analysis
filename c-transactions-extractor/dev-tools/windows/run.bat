@echo off
set project_root=C:/projects/git-test

cd "%project_root%/bin/win32"
call git-test.exe C:/projects/git-test/test-projects/libgit2 ../../output/libgit2-100-txns.json 150
cd "%project_root%/dev-tools/windows"

@echo off

set project_root=C:/projects/git-test

g++ -g -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-strict-aliasing ^
 -I"%project_root%/code" ^
 -IC:\Libs\libgit2-0.28.1\include ^
 "%project_root%/code/main.cpp" ^
 -LC:\Libs\libgit2-0.28.1\build ^
 -lgit2.dll ^
 -o "%project_root%/bin/win32/git-test.exe"

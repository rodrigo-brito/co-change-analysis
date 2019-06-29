project_root=/home/davidoro/projects/git-test

g++ -g -std=c++11 -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-strict-aliasing \
-I"$project_root/code" \
"$project_root/code/main.cpp" \
-o "$project_root/bin/linux/git-test.run"

#!/bin/bash
set -e

target_run_name=lrtcserver
SCRIPT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)
echo "SCRIPT_DIR = " $SCRIPT_DIR
# 获取 CPU 核心数
cpu_cores=$(nproc)

# 判断当前目录是否存在 out 目录
if [ ! -d "out" ]; then
    echo "Creating 'out' directory..."
    mkdir out || { echo "Failed to create 'out' directory."; exit 1; }
else
    echo "'out' directory already exists."
fi

# 进入 out 目录
cd out || { echo "Failed to change directory to 'out'."; exit 1; }

# 执行 cmake 命令
echo "Running cmake..."
cmake .. || { echo "CMake failed."; exit 1; }


if test $# -gt 0 && test $1 = "clean"
then 
    echo "make clean ..."
    make clean
else
    echo "make ..."
    # 执行 make 命令
    make -j$(($cpu_cores * 2))
fi

cd ../
# 检查是否传递了第二个参数，如果是 "-run"，则执行目标运行文件
if [ $# -gt 1 ] && [ "$2" = "-run" ]; then
    echo "Running $target_run_name..."
    ./out/"$target_run_name"
fi



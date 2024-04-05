#!/bin/bash
set -e

target_run_name=lrtcserver
SCRIPT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)
echo "SCRIPT_DIR = " $SCRIPT_DIR
# 获取 CPU 核心数
cpu_cores=$(nproc)

# 定义函数，检查指定端口是否被占用并查找进程
function check_port_and_process() {
    local port=$1
    local port_in_use=$(netstat -tln | grep ":$port")

    if [ -n "$port_in_use" ]; then
        # 指定端口被占用，查找名称包含 "signaling" 的进程并杀死它们
        local signaling_processes=$(netstat -tlnp | grep ":$port" | grep "lrtcs" | awk '{print $7}' | cut -d'/' -f1)

        if [ -n "$signaling_processes" ]; then
            echo "Processes containing 'signaling' listening on port $port have been found."
            echo "Killing processes:"
            echo "$signaling_processes"
            kill -9 $signaling_processes
        else
            echo "No processes containing 'signaling' listening on port $port."
        fi
    else
        echo "Port $port is not in use."
    fi
}
# 编译 libuv jsoncpp libwebrtc  abseil-cpp
# cd${SCRIPT_DIR}/thirdpart/jsoncpp
# cmake .. -DCMAKE_INSTALL_PREFIX=${SCRIPT_DIR}/third_party/jsoncpp
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
    check_port_and_process 9000
    check_port_and_process 9001
    echo "Running $target_run_name..."
    ./out/"$target_run_name"
fi



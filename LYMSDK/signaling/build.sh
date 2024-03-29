#!/bin/bash
# 设置错误发生时退出
set -e

export GO111MODULE="on"
export GOFLAGS=-v

# 获取 CentOS 版本信息
centos_version=$(cat /etc/centos-release)

# 打印 CentOS 版本信息
echo "CentOS version: $centos_version"
# 清除之前的构建文件（如果有的话）
# rm -f signaling
go env -w GO111MODULE="on"
echo "GOPATH = "${GOPATH} "GO111MODULE" ${GO111MODULE}

# 定义函数，检查指定端口是否被占用并查找进程
function check_port_and_process() {
    local port=$1
    local port_in_use=$(netstat -tln | grep ":$port")

    if [ -n "$port_in_use" ]; then
        # 指定端口被占用，查找名称包含 "signaling" 的进程并杀死它们
        local signaling_processes=$(netstat -tlnp | grep ":$port" | grep "signaling" | awk '{print $7}' | cut -d'/' -f1)

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


# 检查当前目录是否存在名为 "signaling" 的文件
if [ -f "signaling" ]; then
    # 如果存在，删除它
    rm signaling
    echo "程序 'signaling' has been deleted."
else
    echo "程序 'signaling' not found."
fi

echo "build ............"
# # 查找并构建 Go 源文件
# find src -name "*.go" -type f -print0 | xargs -0 go build -o signaling
go build -o  signaling src/*.go

echo "run check ............"
# 调用函数，检查8080端口是否被占用
check_port_and_process 8080

# 调用函数，检查8081端口是否被占用
check_port_and_process 8081
echo "run ............"
./signaling
#!/bin/bash
# 设置错误发生时退出
set -e

export GO111MODULE="on"
# 清除之前的构建文件（如果有的话）
# rm -f signaling
go env -w GO111MODULE="on"
echo "GOPATH = "${GOPATH} "GO111MODULE" ${GO111MODULE}

# 检查当前目录是否存在名为 "signaling" 的文件
if [ -f "signaling" ]; then
    # 如果存在，删除它
    rm signaling
    echo "File 'signaling' has been deleted."
else
    echo "File 'signaling' not found."
fi
# # 查找并构建 Go 源文件
# find src -name "*.go" -type f -print0 | xargs -0 go build -o signaling
go build -o  signaling src/*.go

echo "run ............"
# 检查8080端口是否被占用
PORT_IN_USE=$(netstat -tln | grep ":8080")

if [ -n "$PORT_IN_USE" ]; then
    # 8080端口被占用，查找名称包含 "signaling" 的进程并杀死它们
    SIGNALING_PROCESSES=$(netstat -tlnp | grep ":8080" | grep "signaling" | awk '{print $7}' | cut -d'/' -f1)
    
    if [ -n "$SIGNALING_PROCESSES" ]; then
        echo "Processes containing 'signaling' listening on port 8080 have been found."
        echo "Killing processes:"
        echo "$SIGNALING_PROCESSES"
        kill -9 $SIGNALING_PROCESSES
    else
        echo "No processes containing 'signaling' listening on port 8080."
    fi
else
    echo "Port 8080 is not in use."
fi
./signaling
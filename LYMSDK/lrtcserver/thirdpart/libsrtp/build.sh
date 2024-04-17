#!/bin/bash
set -e
# 获取当前脚本所在目录
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# 设置 OpenSSL 源代码和安装目录
SRTP_SOURCE_DIR="$SCRIPT_DIR/srtp"
SRTP_INSTALL_DIR="$SCRIPT_DIR/install"


# 进入 OpenSSL 源代码目录
cd $SRTP_SOURCE_DIR

echo "Building OpenSSL..."$pwd

# 运行 Configure 脚本，指定安装目录
./configure --prefix=$SRTP_INSTALL_DIR

# 执行 make 编译
make

# 执行 make install 安装到指定目录
echo "Installing Srtp..."
make install
echo "Srtp build completed."

# 返回到脚本所在目录
cd $SCRIPT_DIR

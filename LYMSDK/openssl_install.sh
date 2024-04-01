#!/bin/bash
set -e
# OpenSSL 版本
openssl_version="1.0.2e"
target_path=/usr/local/openssl
# 获取 CPU 核心数量
cpu_cores=$(nproc)
# 函数定义
find_matching_files() {
    local directory="$1"
    local pattern="$2"
    find "$directory" -type f -name "$pattern" -exec basename {} \;
}
# 检查openssl是否已经安装
echo "1. 查看主机openssl版本信息："
echo "1.1 查看openssl安装路径："
which openssl
echo ""
echo "1.2 查看openssl版本："
openssl version
echo ""
echo "1.3 查看CentOS版本："
cat /etc/redhat-release
echo ""

# 下载并安装 OpenSSL
echo "2. 下载并安装 OpenSSL："
echo "2.1 下载 OpenSSL 源码包："
wget "https://www.openssl.org/source/old/${openssl_version}/openssl-${openssl_version}.tar.gz"
echo ""

echo "2.2 解压源码包并切换目录："
tar -zxvf "openssl-${openssl_version}.tar.gz"
cd "openssl-${openssl_version}"
echo ""

echo "2.3 配置 OpenSSL 安装路径："
./config --prefix=${target_path}
echo ""

echo "2.4 编译 OpenSSL："
make -j$cpu_cores
echo ""

echo "2.5 安装 OpenSSL："
make install
echo ""

# 切换 openssl 版本
echo "3. 切换 OpenSSL 版本："
echo "3.1 备份旧版本："
mv /usr/bin/openssl /usr/bin/openssl.bak
mv /usr/include/openssl /usr/include/openssl.bak
echo ""

echo "3.2 创建符号链接："
ln -s ${target_path}/bin/openssl /usr/bin/openssl
ln -s ${target_path}/include/openssl /usr/include/openssl
echo ""

echo "3.3 更新库路径："
echo "${target_path}/lib" >> /etc/ld.so.conf
ldconfig -v
echo ""

# echo "3.4 创建动态链接库："
# files=$(find_matching_files "${target_path}/lib/" "libssl.so.*")

# ln -s ${target_path}/lib/libssl.so.1.1 /usr/lib64/libssl.so.1.1
# ln -s ${target_path}/lib/libcrypto.so.1.1 /usr/lib64/libcrypto.so.1.1
# echo ""

echo "操作完成。"


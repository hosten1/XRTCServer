cmake_minimum_required(VERSION 2.19)
project(rtcbase)


# 支持32位/64位平台编译
# set(ENABLE_MULTI_LIBS True)

# C预处理器参数
add_definitions("-g -pipe -W -Wall -fPIC -std=gnu++14 -DWEBRTC_POSIX -DWEBRTC_LINUX")

# 头文件路径
include_directories(
    ./include 
    ./deps/libev/include
)

# 链接库
link_libraries(pthread crypto rt)

# 静态库include目录前缀
set(DEFAULT_LIB_INCLUDE_DIR "rtcbase")

# 用户源文件和头文件
file(GLOB user_sources 
    "./src/*.cpp"
    "./src/*.c"
    "./src/*.cc"
)
file(GLOB user_headers "src/*.h")

# 静态库
add_library(rtcbase STATIC ${user_sources})
# 导出头文件路径
target_include_directories(rtcbase PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/libev/include
)

# 可执行文件
# add_executable(rtcbase ${user_sources})
# 共享库
# add_library(rtcbase SHARED ${user_sources})
# 子目录
# add_subdirectory(demo)

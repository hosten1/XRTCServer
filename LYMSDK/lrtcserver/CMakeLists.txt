cmake_minimum_required(VERSION 2.20)
project(lrtcserver )
# 设置版本变量
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)

add_definitions("-g -pipe -W -Wall -fPIC -std=gnu++14 -DWEBRTC_POSIX -DWEBRTC_LINUX")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
# add_subdirectory("./thirdpart/librtcbase")
# add_subdirectory("./thirdpart/yaml-cpp")
# add_subdirectory("./thirdpart/abseil-cpp")

# 头文件路径
include_directories(
    "./src"
    "./thirdpart/libwebrtc/libwebrtc"
    "./third_party/yaml-cpp/include"
    "./third_party/abseil-cpp/include"
    "./third_party/jsoncpp/include"
    # "./third_party/libuv/include"
    "./third_party/libev/include"
)
# 链接库路径
link_directories(
    "./thirdpart/libwebrtc/out"
    "./third_party/yaml-cpp/lib64"
    "./third_party/abseil-cpp/lib64"
    "./third_party/jsoncpp/lib64"
    # "./third_party/libuv/include"
    "./third_party/libev/lib"
    )

file(GLOB all_src
    "./src/*.cpp"
    "./src/base/*.cpp"
    # "./src/base/handles/*.cpp"
    "./src/server/*.cpp"
    "./src/stream/*.cpp"
    "./src/pc/*.cpp"
   )    

add_executable(lrtcserver ${all_src})

target_link_libraries(lrtcserver 
                    pthread
                    libwebrtc
                    yaml-cpp
                    # uv
                    ev
                    jsoncpp
                    absl_strings
                    absl_strerror
)
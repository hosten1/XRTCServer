cmake_minimum_required(VERSION 2.20)
project(lrtcserver)

add_definitions("-g -pipe -W -Wall -fPIC -std=gnu++14 -DWEBRTC_POSIX -DWEBRTC_LINUX")

# add_subdirectory("./thirdpart/librtcbase")
# add_subdirectory("./thirdpart/yaml-cpp")
# add_subdirectory("./thirdpart/abseil-cpp")

# 头文件路径
include_directories(
    "./src"
    "./thirdpart/libwebrtc/libwebrtc"
    "./third_party/yaml-cpp/include"
    "./third_party/abseil-cpp/include"
)
# 链接库路径
link_directories(
    "./thirdpart/libwebrtc/out"
    "./third_party/yaml-cpp/lib64"
    "./third_party/abseil-cpp/lib64"
    )

file(GLOB all_src
    "./src/*.cpp"
    "./src/base/*.cpp"

   )    

add_executable(lrtcserver ${all_src})

target_link_libraries(lrtcserver 
                    libwebrtc
                    yaml-cpp
                    absl_strings
                    absl_strerror
)
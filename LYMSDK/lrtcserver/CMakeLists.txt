cmake_minimum_required(VERSION 2.20)
project(lrtcserver)

add_definitions("-g -pipe -W -Wall -fPIC -std=gnu++14 -DWEBRTC_POSIX -DWEBRTC_LINUX")

file(GLOB all_src
    "./src/*.cpp"
   )    
add_executable(lrtcserver ${all_src})


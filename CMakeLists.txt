cmake_minimum_required(VERSION 2.6)
project(feather)

add_definitions("-g -fno-omit-frame-pointer")

include_directories(include)
add_subdirectory(src)
add_subdirectory(tests)

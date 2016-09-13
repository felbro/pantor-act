project(act)
cmake_minimum_required (VERSION 2.6)
add_definitions (-std=c++11 -Wall)

add_subdirectory(core)
include_directories(core)

add_subdirectory(tools)
add_subdirectory(solutions)

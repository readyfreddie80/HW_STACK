cmake_minimum_required(VERSION 3.13)
project(HW_STACK)

set(CMAKE_CXX_STANDARD 14)

add_executable(HW_STACK main.cpp stack_t.h stack_t.cpp stdfuncs_wrappers.h stdfuncs_wrappers.cpp)
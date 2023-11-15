cmake_minimum_required(VERSION 3.21)

project(log-compiler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O6")

set(TARGET_NAME log-compiler-executable)

add_subdirectory(src)
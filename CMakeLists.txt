cmake_minimum_required(VERSION 3.13)

project(log-compiler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

set(TARGET_NAME log-compiler)

file(GLOB_RECURSE SOURCES
    "src/*.hpp"
    "src/*.cpp"
)

add_executable (${TARGET_NAME} ${SOURCES})
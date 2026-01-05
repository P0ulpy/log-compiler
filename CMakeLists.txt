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

execute_process(
    COMMAND git rev-parse HEAD
    OUTPUT_VARIABLE LOG_COMPILER_GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
target_compile_definitions(${TARGET_NAME} PUBLIC LOG_COMPILER_GIT_COMMIT_HASH="${LOG_COMPILER_GIT_COMMIT_HASH}")
message(STATUS "GIT_COMMIT_HASH=${LOG_COMPILER_GIT_COMMIT_HASH}")

include(third-party/third-party.cmake)
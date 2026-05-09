cmake_minimum_required(VERSION 3.21)

project(log-compiler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

set(TARGET_NAME log-compiler)

file(GLOB_RECURSE SOURCES
    "src/*.hpp"
    "src/*.cpp"
)

add_executable(${TARGET_NAME} ${SOURCES})

# Test runner
set(TEST_TARGET_NAME log-compiler-tests)

file(GLOB_RECURSE LIB_SOURCES
    "src/Compilation/*.hpp"
    "src/Compilation/*.cpp"
    "src/Utils/*.hpp"
    "src/Utils/*.cpp"
    "src/CLI.hpp"
    "src/CLI.cpp"
    "src/main.hpp"
)

add_executable(${TEST_TARGET_NAME} 
    tests/test_runner.cpp
    ${LIB_SOURCES}
)

# CTest integration
enable_testing()

# Full test suite - replay all formats
add_test(
    NAME test_replay_all
    COMMAND ${TEST_TARGET_NAME} replay --dir ${CMAKE_SOURCE_DIR}/tests/cases
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Per-format tests
add_test(
    NAME test_replay_json
    COMMAND ${TEST_TARGET_NAME} replay --dir ${CMAKE_SOURCE_DIR}/tests/cases --format json
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_test(
    NAME test_replay_md
    COMMAND ${TEST_TARGET_NAME} replay --dir ${CMAKE_SOURCE_DIR}/tests/cases --format md
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Debug output test - exercises operator<< paths
add_test(
    NAME test_debug_output
    COMMAND ${TEST_TARGET_NAME} replay --dir ${CMAKE_SOURCE_DIR}/tests/cases --debug-output
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Code coverage target (Windows only, requires OpenCppCoverage)
find_program(OPENCPPCOVERAGE_EXE OpenCppCoverage)

if(OPENCPPCOVERAGE_EXE)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/src" COVERAGE_SOURCES_DIR)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/out/coverage" COVERAGE_REPORT_DIR)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/out/coverage/coverage.xml" COVERAGE_XML_PATH)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/tests/cases" COVERAGE_TESTS_DIR)

    add_custom_target(coverage
        COMMAND ${OPENCPPCOVERAGE_EXE}
            --sources "${COVERAGE_SOURCES_DIR}"
            --export_type "html:${COVERAGE_REPORT_DIR}"
            --export_type "cobertura:${COVERAGE_XML_PATH}"
            -- "$<TARGET_FILE:${TEST_TARGET_NAME}>" replay --dir "${COVERAGE_TESTS_DIR}" --debug-output --feature-coverage
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        DEPENDS ${TEST_TARGET_NAME}
        COMMENT "Running code coverage with OpenCppCoverage..."
    )
else()
    add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -E echo "ERROR: OpenCppCoverage not found. Install with: winget install OpenCppCoverage.OpenCppCoverage"
        COMMAND ${CMAKE_COMMAND} -E false
    )
endif()
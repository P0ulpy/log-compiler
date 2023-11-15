file(GLOB_RECURSE SOURCES
    "*.hpp"
    "*.cpp"
)

add_executable (${TARGET_NAME} ${SOURCES})
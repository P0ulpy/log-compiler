include(FetchContent)

set(RAYLIB_VERSION 5.5)
set(RAYLIB_LIB raylib)

FetchContent_Declare(
    ${RAYLIB_LIB}
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG ${RAYLIB_VERSION}
    GIT_PROGRESS TRUE
)

set(BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(BUILD_TESTING OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(${RAYLIB_LIB})

set(RAYGUI_INCLUDE ${CMAKE_SOURCE_DIR}/third-party/raygui)

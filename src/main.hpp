#pragma once

#include <cstdint>
#include <memory>

#define LOG_COMPILER_VERSION_MAJOR 0
#define LOG_COMPILER_VERSION_MINOR 1
#define LOG_COMPILER_VERSION_PATCH 1
#define LOG_COMPILER_VERSION_NAME "alpha"

#ifndef LOG_COMPILER_GIT_COMMIT_HASH // Comes from the build system
    #define LOG_COMPILER_GIT_COMMIT_HASH "Unknow"
#endif

struct CompilerOptions
{
    static inline bool Debug = false;
    static inline bool Verbose = false;
    static inline bool RenderMode = false;

    enum class OutputFormats { JSON, Markdown, C };
    static inline OutputFormats OutputFormat = OutputFormats::JSON;
};

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}
#pragma once

#include <memory>

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 0
#define VERSION_TYPE "alpha"

namespace CompilerOptions
{
    static bool DebugMode = false;
    static bool Verbose = false;

    enum class OutputFormats { JSON, Markdown, C };
    static OutputFormats OutputFormat = OutputFormats::JSON;
}

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
#pragma once

#include <string>
#include <string_view>
#include <functional>

#include "../Compilation/Token.hpp"

std::string FormatSpecialCharacters(const std::string_view& str);
std::string FormatToJsonString(const std::string_view& str);

std::string TokenError(const Token& token);

// Note : std::isspace can easily have undefined behaviour with none ASCII characters, use an other method if we update the compiler to support non ASCII characters

// trim from start (in place)
static inline void lTrim(std::string &s) 
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rTrim(std::string &s) 
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void Trim(std::string &s) 
{
    rTrim(s);
    lTrim(s);
}

// trim from start (copying)
static inline std::string lTrimCopy(std::string s) 
{
    lTrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rTrimCopy(std::string s) 
{
    rTrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string TrimCopy(std::string s) 
{
    Trim(s);
    return s;
}
#pragma once

#include <string>
#include <string_view>

#include "../Compilation/Token.hpp"

std::string FormatSpecialCharacters(const std::string_view& str);
std::string FormatToJsonString(const std::string_view& str);

std::string TokenError(const Token& token);
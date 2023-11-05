#pragma once

#include <string>

#include "../Compilation/Token.hpp"

std::string FormatSpecialCharacters(const std::string& str);

std::string TokenError(const Token& token);
#pragma once

#include <string>
#include <sstream>

std::string FormatSpecialCharacters(const std::string& str)
{
    std::stringstream builder;

    for(char c : str)
    {
        switch(c)
        {
            case '\n' : builder << "\\n"; break;
            case '\r' : builder << "\\r"; break;
            case '\t' : builder << "\\t"; break;
            case '\v' : builder << "\\v"; break;
            default:
                builder << c;
        }
    }

    return builder.str();
}
#include "StringUtils.hpp"

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

std::string TokenError(const Token &token)
{
    std::stringstream builder;
    
    const size_t displayedErrorPartMaxSize = 20;
    std::string errorPart = (token.value.size() < displayedErrorPartMaxSize) 
        ? ((token.value.empty()) ? "" : token.value) 
        : token.value.substr(0, displayedErrorPartMaxSize);

    builder
        << '`' << TokenTypeToCstr(token.type) << '`' << '\n'
        << "at line " << token.ln << ", column " << token.col << " -> `" << errorPart << '`';

    return builder.str();

}

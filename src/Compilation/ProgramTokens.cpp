#include "ProgramTokens.hpp"

#include <string>
#include <iostream>

std::ostream &operator<<(std::ostream &os, const TextBlockToken &token)
{
    os << "TextBlockToken : `" << FormatSpecialCharacters(token.text) << '`';
    return os;
}

std::ostream &operator<<(std::ostream &os, const TitleToken &token)
{
    os << "TitleToken : level `" << (uint16_t)token.level << "` content: `" << FormatSpecialCharacters(token.text) << '`';
    return os;
}

std::ostream &operator<<(std::ostream &os, const QuoteBlockToken &token)
{
    os << "QuoteBlockToken : `" << FormatSpecialCharacters(token.text) << '`';
    return os;
}

std::ostream &operator<<(std::ostream &os, const NodeToken &token)
{
    uint16_t level = token.title.level;
    std::string indent;
    for(uint16_t i = 0; i < level; ++i) indent += "    ";

    os << indent << token.title << " with " << token.content.size() << " children" << '\n';

    indent += "  - ";

    for(auto& subTokenVariant : token.content)
    {      
        std::visit([&](auto&& subToken) 
        {
            using Type = std::decay_t<decltype(subToken)>;

            if constexpr (!std::is_same_v<NodeToken, Type>) 
            {
                os << indent << subToken << '\n';
            }
            else
            {
                os << subToken;
            }
        
        }, subTokenVariant);
    }

    return os;
}

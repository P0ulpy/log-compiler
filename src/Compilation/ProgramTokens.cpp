#include "ProgramTokens.hpp"

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
    return os;
}

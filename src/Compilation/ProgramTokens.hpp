#pragma once

#include <vector>

#include "../main.hpp"
#include "../Utils/TypeList.hpp"
#include "../Utils/StringUtils.hpp"
#include "Token.hpp"

struct TextBlockToken
{
    std::string text;
};

std::ostream& operator<<(std::ostream& os, const TextBlockToken& token)
{
    os << "TextBlockToken : `" << FormatSpecialCharacters(token.text) << '`';
    return os;
}

struct TitleToken
{
    // Title of level 0 is invalid, assert this at Parsing (should be impossible but throw an error anyway to tell Tokanization messed up)
    uint16_t level = 0;
    std::string text;
};

std::ostream& operator<<(std::ostream& os, const TitleToken& token)
{
    os << "TitleToken : level `" << (uint16_t)token.level << "` content: `" << FormatSpecialCharacters(token.text) << '`';
    return os;
}

struct QuoteBlockToken
{
    // QuoteBlock with 0 elements is invalid, assert this at Parsing (should be impossible but throw an error anyway to tell Tokanization messed up)    
    std::string text;
};

std::ostream& operator<<(std::ostream& os, const QuoteBlockToken& token)
{
    os << "QuoteBlockToken : `" << FormatSpecialCharacters(token.text) << '`';
    return os;
}

using ProgramTokensTypes = TypeList<
    TextBlockToken,
    TitleToken,
    QuoteBlockToken
>;

struct NodeToken
{
    TitleToken title;
    std::vector<ProgramTokensTypes::ToVariant_t> content;
};

std::ostream& operator<<(std::ostream& os, const NodeToken& token)
{
    return os;
}

using ProgramNodeTypes = TypeList<
    NodeToken
>;

using AllProgramTokensTypes = ProgramTokensTypes::Concat_t<ProgramNodeTypes>;
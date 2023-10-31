#pragma once

#include <vector>

#include "../main.hpp"
#include "../Utils/TypeList.hpp"
#include "Token.hpp"

struct TextBlockToken
{
    std::string text;
};

std::ostream& operator<<(std::ostream& os, const TextBlockToken& token)
{
    os << "TextBlockToken : `" << token.text << '`';
    return os;
}

struct TitleToken
{
    // Title of level 0 is invalid, assert this at Parsing (should be impossible but throw an error anyway to tell Tokanization messed up)
    uint8_t level = 0;
    Token textLiteral;
};

std::ostream& operator<<(std::ostream& os, const TitleToken& token)
{
    return os;
}

struct QuoteBlockToken
{
    // QuoteBlock with 0 elements is invalid, assert this at Parsing (should be impossible but throw an error anyway to tell Tokanization messed up)    
    std::vector<Token> textLiterals;
};

std::ostream& operator<<(std::ostream& os, const QuoteBlockToken& token)
{
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
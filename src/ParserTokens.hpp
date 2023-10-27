#pragma once

#include <string>
#include <vector>

#include "main.hpp"
#include "TypeList.hpp"

struct TextLineToken
{
    Token textLine;
};

struct TitleToken
{
    // Title of level 0 is invalid, assert this at Parsing (should be impossible but throw an error anyway to tell Tokanization messed up)
    uint8_t level = 0;
    Token textLiteral;
};

struct QuoteBlockToken
{
    // QuoteBlock with 0 elements is invalid, assert this at Parsing (should be impossible but throw an error anyway to tell Tokanization messed up)    
    std::vector<Token> textLiterals;
};

using ParserTokensTypes = FundamentalTypeList<
    Scope<TextLineToken>,
    Scope<TitleToken>,
    Scope<QuoteBlockToken>
>;
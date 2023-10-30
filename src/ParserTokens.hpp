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

struct NodeToken
{
    Scope<TitleToken> title;
    std::vector<
        std::variant<
            Scope<TextLineToken>,
            Scope<TitleToken>,
            Scope<QuoteBlockToken>
        >
    > content;
};

using ParserTokensTypes = TypeList<
    Scope<TextLineToken>,
    Scope<TitleToken>,
    Scope<QuoteBlockToken>,
    Scope<NodeToken>
>;

using NotNodeTypes = TypeList<
    Scope<NodeToken>
>;

using a = TypeList<int, bool>;
using b = TypeList<int, char>;

using NodeTypes = TypeListSubtraction<a, b>::type;
#pragma once

#include <string>
#include <sstream>

enum class TokenType
{
    EmptyLine,          // An empty line containing only one or more `\s`, `\n` or `\r` characters
    TextLine,           // A not empty line not Tokenized as an other token
    TextLiteral,        // Any sequence of characters except end of line
    TitleSymbol,        // `#{1,6}` usually followed by the name of the note as a `TextLiteral` (exemple : `# My super duper note`)
    QuoteLine,          // `> ` followed by the content of the QuoteLine (exemple : `> My super duper quoteblock line`)
};

const char* TokenTypeToCstr(TokenType tokenType)
{
    switch(tokenType)
    {
        case TokenType::EmptyLine :         return "EmptyLine";
        case TokenType::TextLine :          return "TextLine";
        case TokenType::TextLiteral :       return "TextLiteral";
        case TokenType::TitleSymbol :       return "TitleSymbol";
        case TokenType::QuoteLine :         return "QuoteLine";
    }
}

struct Token
{
    TokenType type;
    std::string value;
    uint32_t ln = 0, col = 0;
};

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os
        << "{ "
        << "Type: `" << TokenTypeToCstr(token.type) << '`'
        << ", Value: `" << token.value << '`' 
        << ", pos: " << token.ln << ':' << token.col
        << " }";
    return os;
}
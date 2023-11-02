#include "Token.hpp"

const char *TokenTypeToCstr(TokenType tokenType)
{
    switch(tokenType)
    {
        case TokenType::EmptyLine :         return "EmptyLine";
        case TokenType::TextLine :          return "TextLine";
        case TokenType::TextLiteral :       return "TextLiteral";
        case TokenType::TitleSymbol :       return "TitleSymbol";
        case TokenType::QuoteBlockLine :         return "QuoteLine";
    }

    return "INVALID TokenType";
}

std::ostream &operator<<(std::ostream &os, const Token &token)
{
    os
        << "{ "
        << "Type: `" << TokenTypeToCstr(token.type) << '`'
        << ", Value: `" << token.value << '`' 
        << ", pos: " << token.ln << ':' << token.col
        << " }";
    return os;
}
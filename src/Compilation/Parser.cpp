#include "Parser.hpp"

#include "../main.hpp"

#include <iostream>
#include <cassert>
#include <limits>

Parser::Parser(std::vector<Token>& tokens)
    : m_tokens(tokens)
{}

ProgramRoot Parser::ParseProgram(const std::string_view& name)
{
    ProgramRoot rootNode {
        .name = std::string(name),
    };

    while(CanPeek())
    {
        if(auto programToken = ParseNext())
        {
            rootNode.content.push_back(programToken.value());
        }
    }

    return rootNode;
}

/** TODO :
 * Introduce some kind of a visitor for TryConsume() like in Generation 
*/
const std::optional<ProgramTokenVariant> Parser::ParseNext()
{
    if(auto textLine = TryConsume(TokenType::TextLine)) 
    {
        TextBlockToken textBlockToken {};

        textBlockToken.lines.push_back(textLine.value().value);

        Token& previousToken = textLine.value();

        while(const auto& subTextLine = TryConsume(TokenType::TextLine))
        {           
            if(previousToken.value.ends_with('\\'))
            {
                // remove trailing slash from prev string
                textBlockToken.lines.back().pop_back();
                textBlockToken.lines.push_back(subTextLine.value().value);
            }
            else
            {
                textBlockToken.lines.back().append(' ' + subTextLine.value().value);
            }
            
            previousToken = subTextLine.value();
        }

        return textBlockToken;
    }
    else if(auto titleSymbolOpt = TryConsume(TokenType::TitleSymbol))
    {
        // Title token

        uint16_t titleLevel = ComputeTitleLevel(titleSymbolOpt.value());

        TitleToken titleToken {
            .level = titleLevel
        };

        auto textLiteral = MustConsume(TokenType::TextLiteral);
        titleToken.text = textLiteral.value().value;

        // Content tokens

        NodeToken nodeToken {
            .title = titleToken
        };

        while(CanPeek())
        {
            if(auto titleSymbolOpt = TryPeek(TokenType::TitleSymbol))
            {
                uint16_t innerTitleLevel = ComputeTitleLevel(titleSymbolOpt.value());

                if(innerTitleLevel <= titleLevel)
                    return nodeToken;
            }

            if(auto programTokenOtp = ParseNext())
            {
                nodeToken.content.push_back(programTokenOtp.value());
            }
        }

        return nodeToken;
    }
    else if(auto quoteLine = TryConsume(TokenType::QuoteBlockLine))
    {
        QuoteBlockToken quoteBlockToken {};

        quoteBlockToken.lines.push_back(quoteLine.value().value);

        while(const auto& subQuoteLine = TryConsume(TokenType::QuoteBlockLine))
        {
            quoteBlockToken.lines.push_back(subQuoteLine.value().value);
        }

        return quoteBlockToken;
    }
    else if(auto emptyLine = TryConsume(TokenType::EmptyLine))
    {
        return {};
    }
    else
    {
        if(CompilerOptions::Debug)
            std::cerr << "Parsing WARN : Unhandled Token, `" << Peek() << '`' << '\n';
        
        Consume();
        return {};
    }

}

uint16_t Parser::ComputeTitleLevel(const Token& titleSymbolToken)
{
    assert(titleSymbolToken.value.size() < std::numeric_limits<uint16_t>::max());
    
    uint16_t titleLevel = static_cast<uint16_t>(titleSymbolToken.value.size());

    if(titleLevel == 0 || titleLevel > 6)
    {
        std::cerr 
            << "Parsing ERROR : Invalid title level : `" << titleSymbolToken 
            << "` for token " << TokenError(titleSymbolToken) 
            << '\n';
            
        exit(EXIT_FAILURE);
    }

    return titleLevel;
}

bool Parser::TryPeek(Token &outToken, int16_t offset) const
{
    if(CanPeek(offset))
    {
        outToken = Peek(offset);
        return true;
    }

    return false;
}

const std::optional<Token> Parser::TryPeek(int16_t offset) const
{
    if(CanPeek(offset))
    {
        return Peek(offset);
    }

    return {};
}

const std::optional<Token> Parser::TryPeek(TokenType type, int16_t offset) const
{
    Token token;
    if(TryPeek(token) && token.type == type)
    {
        return Peek(offset);
    }

    return {};
}

const std::optional<Token> Parser::MustConsume(TokenType type, const std::string &errorMsg)
{
    if(auto tokenOpt = TryPeek())
    {
        auto& token = tokenOpt.value();

        if(token.type != type)
        {
            std::cerr
                << errorMsg << '\n'
                << "Expected token : `" << TokenTypeToCstr(type) << '`' << '\n'
                << "Unexpexted token " << TokenError(token)
                << '\n';
        }
        else
        {
            return Consume();
        }
    }
    else
    {
        std::cerr 
            << errorMsg << '\n' 
            << "Unexpected End of file : Expected `" << TokenTypeToCstr(type) << "` token" << '\n';
        exit(EXIT_FAILURE);
    }

    return {};
}

const std::optional<Token> Parser::TryConsume(TokenType type)
{
    if(TryPeek(type))
    {
        return Consume();
    }

    return {};
}

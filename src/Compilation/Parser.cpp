#include "Parser.hpp"

#include <iostream>

Parser::Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens))
{}

std::vector<ProgramTokenVariant> Parser::ParseProgram()
{
    std::vector<ProgramTokenVariant> program;

    while(CanPeek())
    {
        if(auto programToken = Parse())
        {
            program.push_back(programToken.value());
        }
    }

    return program;
}

std::optional<ProgramTokenVariant> Parser::Parse()
{
    if(auto textLine = TryConsume(TokenType::TextLine))
    {
        TextBlockToken textBlockToken {
            .text = textLine.value().value
        };

        while(auto subTextLine = TryConsume(TokenType::TextLine))
        {
            textBlockToken.text += '\n' + subTextLine.value().value;
        }

        return textBlockToken;
    }
    else if(auto titleSymbolOpt = TryConsume(TokenType::TitleSymbol))
    {
        uint16_t titleLevel = titleSymbolOpt.value().value.size();

        if(titleLevel == 0)
        {
            std::cerr << "ERROR : Parsing error, Invalid title level" << std::endl;
            exit(EXIT_FAILURE);
        }

        TitleToken titleToken {
            .level = titleLevel
        };

        auto textLiteral = ShouldConsume(TokenType::TextLiteral);
        titleToken.text = textLiteral.value().value;

        return titleToken;
    }
    else if(auto quoteLine = TryConsume(TokenType::QuoteBlockLine))
    {
        QuoteBlockToken quoteBlockToken {
            .text = quoteLine.value().value
        };

        while(auto subQuoteLine = TryConsume(TokenType::QuoteBlockLine))
        {
            quoteBlockToken.text += '\n' + subQuoteLine.value().value;
        }

        return quoteBlockToken;
    }
    else if(auto emptyLine = TryConsume(TokenType::EmptyLine))
    {
        return {};
    }
    else
    {
        std::cerr << "ERROR : Unhandled Token, " << Peek() << std::endl;
        Consume();
        return {};
    }
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

std::optional<Token> Parser::TryPeek(int16_t offset) const
{
    if(CanPeek(offset))
    {
        return Peek(offset);
    }

    return {};
}

std::optional<Token> Parser::ShouldConsume(TokenType type, const std::string &errorMsg)
{
    if(auto tokenOpt = TryPeek())
    {
        auto& token = tokenOpt.value();

        if(token.type != type)
        {
            const size_t displayedErrorPartMaxSize = 6;
            std::string errorPart = (token.value.size() < displayedErrorPartMaxSize) 
                ? ((token.value.empty()) ? "" : token.value) 
                : token.value.substr(0, displayedErrorPartMaxSize);

            std::cerr
                << errorMsg << '\n'
                << "Expected token : `" << TokenTypeToCstr(type) << '`' << '\n'
                << "Unexpexted token `" << TokenTypeToCstr(token.type) << '`'
                << " at line " << token.ln << ", column " << token.col << '\n'
                << "-> `" << errorPart << '`'
                << std::endl;
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
            << "Unexpected End of file : Expected `" << TokenTypeToCstr(type) << "` token" << std::endl;
        exit(EXIT_FAILURE);
    }

    return {};
}

std::optional<Token> Parser::TryConsume(TokenType type)
{
    Token token;
    if(TryPeek(token) && token.type == type)
    {
        return Consume();
    }

    return {};
}

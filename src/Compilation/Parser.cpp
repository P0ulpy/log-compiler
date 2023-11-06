#include "Parser.hpp"

#include "../main.hpp"

#include <iostream>

Parser::Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens))
{}

std::vector<ProgramTokenVariant> Parser::ParseProgram()
{
    std::vector<ProgramTokenVariant> program;

    while(CanPeek())
    {
        if(auto programToken = ParseNext())
        {
            program.push_back(programToken.value());
        }
    }

    return program;
}

std::optional<ProgramTokenVariant> Parser::ParseNext()
{
    if(auto textLine = TryConsume(TokenType::TextLine)) 
    {
        TextBlockToken textBlockToken {
            .text = textLine.value().value
        };

        while(auto subTextLine = TryConsume(TokenType::TextLine))
        {
            textBlockToken.text += ' ' + subTextLine.value().value;
        }

        return textBlockToken;
    }
    else if(auto titleSymbolOpt = TryConsume(TokenType::TitleSymbol))
    {
        uint16_t titleLevel = titleSymbolOpt.value().value.size();

        if(titleLevel == 0 || titleLevel > 6)
        {
            std::cerr 
                << "Parsing ERROR : Invalid title level : `" << titleLevel 
                << "` for token " << TokenError(titleSymbolOpt.value()) 
                << std::endl;
                
            exit(EXIT_FAILURE);
        }

        TitleToken titleToken {
            .level = titleLevel
        };

        auto textLiteral = MustConsume(TokenType::TextLiteral);
        titleToken.text = textLiteral.value().value;

        // Content

        NodeToken nodeToken {
            .title = titleToken
        };

        while(CanPeek())
        {
            if(auto programTokenOtp = ParseNext())
            {
                bool isChildToken = true;

                std::visit([&](auto&& programToken)
                {
                    using Type = std::decay_t<decltype(programTokenOtp.value())>;
                    
                    if constexpr (std::is_same_v<NodeToken, Type>)
                    {
                        if(static_cast<const NodeToken&>(programToken).title.level <= titleLevel)
                        {
                            isChildToken = false;
                        }
                    }
                    
                }, programTokenOtp.value());

                if(isChildToken)
                {
                    nodeToken.content.push_back(programTokenOtp.value());
                }
                else
                {
                    return programTokenOtp;
                }
            }
        }

        return nodeToken;
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
        if(CompilerOptions::DebugMode)
            std::cerr << "Parsing WARN : Unhandled Token, `" << Peek() << '`' << std::endl;
        
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

std::optional<Token> Parser::MustConsume(TokenType type, const std::string &errorMsg)
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

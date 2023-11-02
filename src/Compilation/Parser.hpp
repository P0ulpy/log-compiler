#pragma once

#include <vector>
#include <variant>
#include <optional>

#include "Tokenizer.hpp"
#include "ProgramTokens.hpp"
#include "../CLI.hpp"

using ProgramTokenVariant = AllProgramTokensTypes::ToVariant_t;

// NOTE :   To improve the memory continguity of the Pasing phase we could maybe use an ArenaAllocator with a computed size at Parser construction
//          this whould prevent from making a heap allocations for every tokens 
// NOTE for the note : we don't use heap allocation anymore be a ArenaAllocator can still be relevant

class Parser
{
public:
    Parser() = delete;

    Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens))
    {}

    std::vector<ProgramTokenVariant> ParseProgram()
    {
        std::vector<ProgramTokenVariant> program;

        while(CanPeek())
        {
            if(auto programToken = ParseToken())
            {
                program.push_back(programToken.value());
            }
        }

        return program;
    }

    std::optional<ProgramTokenVariant> ParseToken()
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
        else if(auto quoteLine = TryConsume(TokenType::QuoteLine))
        {
            QuoteBlockToken quoteBlockToken {
                .text = quoteLine.value().value
            };

            while(auto subQuoteLine = TryConsume(TokenType::QuoteLine))
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

private:

    [[nodiscard]] inline bool CanPeek(int16_t offset = 0) const
    {
        return m_cursor + offset < m_tokens.size();
    }

    [[nodiscard]] inline Token Peek(int16_t offset = 0) const
    {
        return m_tokens[offset + m_cursor];
    }

    [[nodiscard]] bool TryPeek(Token& outToken, int16_t offset = 0) const
    {
        if(CanPeek(offset))
        {
            outToken = Peek(offset);
            return true;
        }

        return false;
    }

    [[nodiscard]] std::optional<Token> TryPeek(int16_t offset = 0) const
    {
        if(CanPeek(offset))
        {
            return Peek(offset);
        }

        return {};
    }

    inline Token Consume()
    {
        return m_tokens[m_cursor++];
    }

    [[nodiscard]] std::optional<Token> ShouldConsume(TokenType type, const std::string& errorMsg = "Syntax ERROR")
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
    
    [[nodiscard]] std::optional<Token> TryConsume(TokenType type)
    {
        Token token;
        if(TryPeek(token) && token.type == type)
        {
            return Consume();
        }

        return {};
    }

private:
    std::vector<Token> m_tokens;
    size_t m_cursor = 0;
};
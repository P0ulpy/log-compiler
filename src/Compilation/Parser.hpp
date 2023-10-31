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

        Token token;
        while(TryPeek(token))
        {
            if(auto programToken = ParseToken())
            {
                program.push_back(programToken.value());
            }
            else
            {
                std::cerr << "ERROR : Unhandled Token, " << token << std::endl;
                Consume();
            }
        }

        return program;
    }

    std::optional<ProgramTokenVariant> ParseToken()
    {
        if(auto textLine = TryConsume(TokenType::TextLine))
        {
            TextBlockToken textLineToken {
                .text = textLine.value().value
            };

            while(auto subTextLine = TryConsume(TokenType::TextLine))
            {
                textLineToken.text += '\n' + subTextLine.value().value;
            }

            return textLineToken;
        }

        return {};
    }

private:

    [[nodiscard]] bool CanPeek(int16_t offset = 0) const
    {
        return m_cursor + offset < m_tokens.size();
    }

    [[nodiscard]] bool TryPeek(Token& outToken, int16_t offset = 0) const
    {
        if(CanPeek(offset))
        {
            outToken = m_tokens[offset + m_cursor];
            return true;
        }

        return false;
    }

    Token Consume()
    {
        return m_tokens[m_cursor++];
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
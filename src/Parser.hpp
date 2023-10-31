#pragma once

#include <vector>
#include <variant>

#include "Tokenizer.hpp"
#include "ParserTokens.hpp"

using ProgramTokenVariant = AllParserTokensTypes::ToVariant_t;

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

        // TODO : Implement Parsing

        return program;
    }

private:
    std::vector<Token> m_tokens;
};
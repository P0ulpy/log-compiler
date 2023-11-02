#pragma once

#include <vector>
#include <optional>

#include "Token.hpp"
#include "ProgramTokens.hpp"

// NOTE :   To improve the memory continguity of the Pasing phase we could maybe use an ArenaAllocator with a computed size at Parser construction
//          this whould prevent from making a heap allocations for every tokens 
// NOTE for the note : we don't use heap allocation anymore but a ArenaAllocator can still be relevant

class Parser
{
public:
    Parser() = delete;
    Parser(std::vector<Token> tokens);

    [[nodiscard]] std::vector<ProgramTokenVariant> ParseProgram();
    [[nodiscard]] std::optional<ProgramTokenVariant> Parse();

private:

    [[nodiscard]] inline bool CanPeek(int16_t offset = 0) const     { return m_cursor + offset < m_tokens.size(); }
    [[nodiscard]] inline Token Peek(int16_t offset = 0) const       { return m_tokens[offset + m_cursor]; }
    [[nodiscard]] bool TryPeek(Token& outToken, int16_t offset = 0) const;
    [[nodiscard]] std::optional<Token> TryPeek(int16_t offset = 0) const;

    inline Token Consume()                                          { return m_tokens[m_cursor++]; }
    [[nodiscard]] std::optional<Token> TryConsume(TokenType type);
    [[nodiscard]] std::optional<Token> ShouldConsume(TokenType type, const std::string& errorMsg = "Syntax ERROR");

private:
    std::vector<Token> m_tokens;
    size_t m_cursor = 0;
};
#pragma once

#include <vector>
#include <optional>
#include <cstdint>

#include "Token.hpp"
#include "ProgramTokens.hpp"
#include "InlineContent.hpp"
#include "../Utils/Assertion.hpp"

// NOTE :   To improve the memory continguity of the Pasing phase we could maybe use an ArenaAllocator with a computed size at Parser construction
//          this whould prevent from making a heap allocations for every tokens 
// NOTE for the note : we use multiple contiguous heap allocation (with std:vector) now, but a ArenaAllocator can still be relevant

class Parser;

class ParserTokenVisitor
{
public:
    ParserTokenVisitor() = delete;
    ParserTokenVisitor(Parser& parser);

    template<TokenType Type>
    std::optional<ProgramTokenVariant> operator()();

private:
    Parser& m_parser;
};

template <> std::optional<ProgramTokenVariant> ParserTokenVisitor::operator()<TokenType::TextLine>();
template <> std::optional<ProgramTokenVariant> ParserTokenVisitor::operator()<TokenType::TitleSymbol>();
template <> std::optional<ProgramTokenVariant> ParserTokenVisitor::operator()<TokenType::QuoteBlockLine>();
template <> std::optional<ProgramTokenVariant> ParserTokenVisitor::operator()<TokenType::EmptyLine>();
template <> std::optional<ProgramTokenVariant> ParserTokenVisitor::operator()<TokenType::TextLiteral>();

// Default: compile-time error for unhandled token types
template<TokenType Type>
inline std::optional<ProgramTokenVariant> ParserTokenVisitor::operator()()
{
    static_assert(assert_false_v<std::integral_constant<TokenType, Type>>, 
        "Unhandled TokenType in ParserTokenVisitor, please add a specialisation for this type");
    return {};
}

class Parser
{
public:
    Parser() = delete;
    Parser(std::vector<Token>& tokens);

    [[nodiscard]] ProgramRoot ParseProgram(const std::string_view& name = "program");
    [[nodiscard]] const std::optional<ProgramTokenVariant> ParseNext();

private:
    [[nodiscard]] uint16_t ComputeTitleLevel(const Token& titleSymbolToken);

    [[nodiscard]] inline const bool CanPeek(int16_t offset = 0) const     { return m_cursor + offset < m_tokens.size(); }
    [[nodiscard]] inline const Token Peek(int16_t offset = 0) const       { return m_tokens[offset + m_cursor]; }
    [[nodiscard]] bool TryPeek(Token& outToken, int16_t offset = 0) const;
    [[nodiscard]] const std::optional<Token> TryPeek(int16_t offset = 0) const;
    [[nodiscard]] const std::optional<Token> TryPeek(TokenType type, int16_t offset = 0) const;

    inline const Token Consume()                                          { return m_tokens[m_cursor++]; }
    [[nodiscard]] const std::optional<Token> TryConsume(TokenType type);
    [[nodiscard]] const std::optional<Token> MustConsume(TokenType type, const std::string& errorMsg = "Syntax ERROR");

private:
    std::vector<Token>& m_tokens;
    size_t m_cursor = 0;

    friend class ParserTokenVisitor;
};
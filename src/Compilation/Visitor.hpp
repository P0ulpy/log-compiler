#pragma once

#include "Token.hpp"
#include "../Utils/Assertion.hpp"

/**
 * Compile-time visitor for the parsing phase.
 * Uses template specialization with static_assert to guarantee at compile time
 * that every TokenType has a corresponding handler.
 * 
 * Subclass this and specialize operator() for each TokenType your parser supports.
 * Any unhandled TokenType will trigger a compile error.
 */
template<typename TDerived>
class ParserVisitor
{
public:
    template<TokenType Type>
    std::optional<ProgramTokenVariant> operator()();
};

// Default: compile-time error for unhandled token types
// Specializations are provided in Parser.cpp
template<typename TDerived>
template<TokenType Type>
inline std::optional<ProgramTokenVariant> ParserVisitor<TDerived>::operator()()
{
    static_assert(assert_false_v<std::integral_constant<TokenType, Type>>, 
        "Unhandled TokenType, please add a ParserVisitor specialisation for this type");
    return {};
}

/**
 * Compile-time visitor for the tokenization phase.
 * Dispatches line classification to template-specialized handlers.
 * 
 * Each line pattern (empty, escaped, blockquote, title, text) has a corresponding
 * handler. Adding a new line pattern without implementing the handler triggers
 * a compile error.
 */
enum class LinePattern
{
    Empty,
    Escaped,
    Blockquote,
    Title,
    Text,
};

template<typename TDerived>
class TokenizerVisitor
{
public:
    template<LinePattern Pattern>
    void operator()(std::vector<Token>& tokens, const std::string& line, uint32_t lineIndex);
};

template<typename TDerived>
template<LinePattern Pattern>
inline void TokenizerVisitor<TDerived>::operator()(std::vector<Token>& tokens, const std::string& line, uint32_t lineIndex)
{
    static_assert(assert_false_v<std::integral_constant<LinePattern, Pattern>>, 
        "Unhandled LinePattern, please add a TokenizerVisitor specialisation for this pattern");
}

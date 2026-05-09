#pragma once

#include <vector>

#include "Token.hpp"
#include "../Utils/Assertion.hpp"
#include <cstdint>
#include <regex>

/**
 * Line pattern classification for the tokenizer visitor.
 * Adding a new pattern without implementing the visitor specialization
 * triggers a compile-time error.
 */
enum class LinePattern
{
    Empty,
    Escaped,
    Blockquote,
    Title,
    Text,
};

class Tokenizer;

class TokenizerLineVisitor
{
public:
    TokenizerLineVisitor() = delete;
    TokenizerLineVisitor(Tokenizer& tokenizer);

    template<LinePattern Pattern>
    void operator()(std::vector<Token>& tokens, const std::string& line, uint32_t lineIndex, const std::smatch& match);

private:
    Tokenizer& m_tokenizer;
};

template <> void TokenizerLineVisitor::operator()<LinePattern::Empty>(std::vector<Token>& tokens, const std::string& line, uint32_t lineIndex, const std::smatch& match);
template <> void TokenizerLineVisitor::operator()<LinePattern::Escaped>(std::vector<Token>& tokens, const std::string& line, uint32_t lineIndex, const std::smatch& match);
template <> void TokenizerLineVisitor::operator()<LinePattern::Blockquote>(std::vector<Token>& tokens, const std::string& line, uint32_t lineIndex, const std::smatch& match);
template <> void TokenizerLineVisitor::operator()<LinePattern::Title>(std::vector<Token>& tokens, const std::string& line, uint32_t lineIndex, const std::smatch& match);
template <> void TokenizerLineVisitor::operator()<LinePattern::Text>(std::vector<Token>& tokens, const std::string& line, uint32_t lineIndex, const std::smatch& match);

template<LinePattern Pattern>
inline void TokenizerLineVisitor::operator()(std::vector<Token>& tokens, const std::string& line, uint32_t lineIndex, const std::smatch& match)
{
    static_assert(assert_false_v<std::integral_constant<LinePattern, Pattern>>, 
        "Unhandled LinePattern, please add a TokenizerLineVisitor specialisation for this pattern");
}

class Tokenizer
{
public:
    Tokenizer() = delete;
    Tokenizer(std::string source);
    
    std::vector<Token> Tokenize();

private:
    bool CanPeek() const;
    std::string Consume();

    struct EndOfLineInfo { size_t index = 0; uint16_t terminatorOffset = 0; };
    [[nodiscard]] EndOfLineInfo FindEndOfLine(size_t startPos = 0) const;
    
private:
    std::string m_source;
    size_t m_cursor = 0;

    friend class TokenizerLineVisitor;
};
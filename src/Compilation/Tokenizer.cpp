#include "Tokenizer.hpp"

#include <regex>

#include "../Utils/StringUtils.hpp"

Tokenizer::Tokenizer(std::string source)
    : m_source(std::move(source))
{}

std::vector<Token> Tokenizer::Tokenize()
{
    std::vector<Token> tokens;
    
    uint32_t lineIndex = 1;

    TokenizerLineVisitor visitor(*this);
    
    while (CanPeek())
    {
        std::string line = Consume();

        const static std::regex TitleRgx { "(#{1,6}) (.*)" };
        const static std::regex BlockquoteRgx { "(>{1} )(.*)" };
        const static std::regex NotEmptyLineRgx { "(?!( {1,}|\n{1,}|\r{1,}|$))" };

        std::smatch match;

        if(line.size() == 0 || !std::regex_search(line, NotEmptyLineRgx))
        {
            visitor.operator()<LinePattern::Empty>(tokens, line, lineIndex, match);
        }
        else if(line.size() >= 2 && line[0] == '\\' && (line[1] == '#' || line[1] == '>'))
        {
            visitor.operator()<LinePattern::Escaped>(tokens, line, lineIndex, match);
        }
        else if (std::regex_match(line, match, BlockquoteRgx))
        {
            visitor.operator()<LinePattern::Blockquote>(tokens, line, lineIndex, match);
        }
        else if (std::regex_match(line, match, TitleRgx))
        {
            visitor.operator()<LinePattern::Title>(tokens, line, lineIndex, match);
        }
        else
        {
            visitor.operator()<LinePattern::Text>(tokens, line, lineIndex, match);
        }
        
        ++lineIndex;
    }

    return tokens;
}

// --- TokenizerLineVisitor specializations ---

TokenizerLineVisitor::TokenizerLineVisitor(Tokenizer& tokenizer)
    : m_tokenizer(tokenizer)
{}

template <>
void TokenizerLineVisitor::operator()<LinePattern::Empty>(std::vector<Token>& tokens, const std::string& line, uint32_t lineIndex, const std::smatch& match)
{
    // Skip Multiple empty lines
    if(tokens.size() > 0 && tokens.back().type == TokenType::EmptyLine)
        return;
    
    tokens.push_back({
        .type = TokenType::EmptyLine,
        .ln = lineIndex,
        .col = 1U,
    });
}

template <>
void TokenizerLineVisitor::operator()<LinePattern::Escaped>(std::vector<Token>& tokens, const std::string& line, uint32_t lineIndex, const std::smatch& match)
{
    std::string unescaped = line.substr(1);
    tokens.push_back({
        .type = TokenType::TextLine,
        .value = rTrimCopy(unescaped),
        .ln = lineIndex,
        .col = 1U,
    });
}

template <>
void TokenizerLineVisitor::operator()<LinePattern::Blockquote>(std::vector<Token>& tokens, const std::string& line, uint32_t lineIndex, const std::smatch& match)
{
    tokens.push_back({
        .type = TokenType::QuoteBlockLine,
        .value = rTrimCopy(match[2]),
        .ln = lineIndex,
        .col = static_cast<uint32_t>(match.position(2)) + 1,
    });
}

template <>
void TokenizerLineVisitor::operator()<LinePattern::Title>(std::vector<Token>& tokens, const std::string& line, uint32_t lineIndex, const std::smatch& match)
{
    tokens.push_back({
        .type = TokenType::TitleSymbol,
        .value = match[1],
        .ln = lineIndex,
        .col = static_cast<uint32_t>(match.position(1)) + 1,
    });

    tokens.push_back({
        .type = TokenType::TextLiteral,
        .value = TrimCopy(match[2]),
        .ln = lineIndex,
        .col = static_cast<uint32_t>(match.position(2)) + 1,
    });
}

template <>
void TokenizerLineVisitor::operator()<LinePattern::Text>(std::vector<Token>& tokens, const std::string& line, uint32_t lineIndex, const std::smatch& match)
{
    tokens.push_back({
        .type = TokenType::TextLine,
        .value = rTrimCopy(line),
        .ln = lineIndex,
        .col = 1U,
    });
}

// --- Tokenizer utility methods ---

bool Tokenizer::CanPeek() const
{
    return m_cursor < m_source.size();
}

std::string Tokenizer::Consume()
{
    auto endOfLineInfo = FindEndOfLine(m_cursor);
    
    std::string out;

    if (endOfLineInfo.index != std::string::npos)
    {
        out = m_source.substr(m_cursor, endOfLineInfo.index - m_cursor);
        m_cursor = endOfLineInfo.index + endOfLineInfo.terminatorOffset;
    } 
    else 
    {
        out = m_source.substr(m_cursor);
        m_cursor = m_source.size();
    }

    return out;
}

Tokenizer::EndOfLineInfo Tokenizer::FindEndOfLine(size_t startPos) const
{
    size_t length = m_source.length();
    
    for (size_t i = startPos; i < length; ++i) 
    {
        char character = m_source[i];

        if (character == '\n' || character == '\r') 
        {
            if (character == '\r' && (i + 1 < length) && m_source[i + 1] == '\n')
                return { i, 2 };
            else
                return { i, 1 };
        }
    }

    return { std::string::npos, 0 };
}

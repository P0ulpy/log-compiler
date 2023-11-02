#pragma once

#include <vector>
#include <regex>

#include "Token.hpp"

class Tokenizer
{
public:
    Tokenizer() = delete;

    Tokenizer(std::string source) 
        : m_source(std::move(source))
    {}

    std::vector<Token> Tokenize() 
    {
        std::vector<Token> tokens;
        
        uint32_t lineIndex = 1;
        std::string line;
        
        while (TryPeek(line))
        {            
            const static std::regex TitleRgx { "(#{1,6}) (.*)" };
            const static std::regex BlockquoteRgx { "(>{1} )(.*)" };
            // Note : Maybe look for a more efficient regex
            const static std::regex NotEmptyLineRgx { "(?!( {1,}|\n{1,}|\r{1,}|$))" };

            std::smatch match;

            if(line.size() == 0 || !std::regex_search(line, NotEmptyLineRgx))
            {
                tokens.push_back({
                    .type = TokenType::EmptyLine,
                    .ln = lineIndex,
                    .col = 1U,
                });
            }
            else if (std::regex_match(line, match, BlockquoteRgx))
            {
                tokens.push_back({
                    .type = TokenType::QuoteLine,
                    .value = match[2],
                    .ln = lineIndex,
                    .col = static_cast<uint32_t>(match.position(2)) + 1,
                });
            }
            else if (std::regex_match(line, match, TitleRgx))
            {
                tokens.push_back({
                    .type = TokenType::TitleSymbol,
                    .value = match[1],
                    .ln = lineIndex,
                    .col = static_cast<uint32_t>(match.position(1)) + 1,
                });

                tokens.push_back({
                    .type = TokenType::TextLiteral,
                    .value = match[2],
                    .ln = lineIndex,
                    .col = static_cast<uint32_t>(match.position(2)) + 1,
                });
            }
            else
            {
                tokens.push_back({
                    .type = TokenType::TextLine,
                    .value = line,
                    .ln = lineIndex,
                    .col = 1U,
                });
            }
            
            Consume();
            ++lineIndex;
        }

        return tokens;
    }

private:

    [[nodiscard]] bool TryPeek(std::string& line) const
    {
        if (m_cursor >= m_source.size())
        {
            return false;
        }

        size_t lineEndIndex = FindEndOfLine(m_cursor).index;

        if (lineEndIndex != std::string::npos)
        {
            line = m_source.substr(m_cursor, lineEndIndex - m_cursor);
        }
        else 
        {
            // This handles the last line
            line = m_source.substr(m_cursor);
        }

        return true;
    }

    void Consume()
    {
        auto endOfLineInfo = FindEndOfLine(m_cursor);
        
        if (endOfLineInfo.index != std::string::npos)
        {
            m_cursor = endOfLineInfo.index + endOfLineInfo.terminatorOffset;
        } 
        else 
        {
            m_cursor = m_source.size();
        }
    }

    struct EndOfLineInfo { size_t index = 0; uint16_t terminatorOffset = 0; };
    [[nodiscard]] EndOfLineInfo FindEndOfLine(size_t startPos = 0) const
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

private:

    std::string m_source;
    size_t m_cursor = 0;
};
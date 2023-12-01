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
    
    while (CanPeek())
    {
        std::string line = Consume();

        const static std::regex TitleRgx { "(#{1,6}) (.*)" };
        const static std::regex BlockquoteRgx { "(>{1} )(.*)" };
        // NOTE : Maybe look for a more efficient regex
        const static std::regex NotEmptyLineRgx { "(?!( {1,}|\n{1,}|\r{1,}|$))" };

        std::smatch match;

        if(line.size() == 0 || !std::regex_search(line, NotEmptyLineRgx))
        {
            // Skip Multiple empty line
            if(tokens.size() > 0 && tokens.back().type == TokenType::EmptyLine)
                continue;
            
            tokens.push_back({
                .type = TokenType::EmptyLine,
                .ln = lineIndex,
                .col = 1U,
            });
        }
        else if (std::regex_match(line, match, BlockquoteRgx))
        {
            tokens.push_back({
                .type = TokenType::QuoteBlockLine,
                .value = rTrimCopy(match[2]),
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
                .value = TrimCopy(match[2]),
                .ln = lineIndex,
                .col = static_cast<uint32_t>(match.position(2)) + 1,
            });
        }
        else
        {
            tokens.push_back({
                .type = TokenType::TextLine,
                .value = rTrimCopy(line),
                .ln = lineIndex,
                .col = 1U,
            });
        }
        
        ++lineIndex;
    }

    return tokens;
}

bool Tokenizer::CanPeek() const
{
    return m_cursor < m_source.size();
}

bool Tokenizer::TryPeek(std::string &line) const
{
    if (CanPeek())
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

#include <vector>
#include <regex>

enum class TokenType
{
    EmptyLine,          // An empty line containing only one or more `\s`, `\n` or `\r` characters
    TextLine,           // A not empty line not Tokenized as an other token
    TextLiteral,        // Any sequence of characters except end of line
    TitleSymbol,        // `#{1,6}` usually followed by the name of the note as a `TextLiteral` (exemple : `# My super duper note`)
    QuoteLine,          // `> ` followed by the content of the QuoteLine (exemple : `> My super duper quoteblock line`)
};

struct Token
{
    TokenType type;
    std::string value;
    uint32_t ln = 0, col = 0;
};

class Tokenizer
{
public:
    Tokenizer(std::string source) 
        : m_source(std::move(source))
        , m_currentIndex(0) 
    {}

    bool TryPeek(std::string& line) 
    {
        if (m_currentIndex >= m_source.size())
        {
            return false;
        }

        size_t lineEndIndex = FindEndOfLine(m_currentIndex).index;

        if (lineEndIndex != std::string::npos)
        {
            line = m_source.substr(m_currentIndex, lineEndIndex - m_currentIndex);
        }
        else 
        {
            // This handles the last line
            line = m_source.substr(m_currentIndex);
        }

        return true;
    }

    void Consume() 
    {
        auto endOfLineInfo = FindEndOfLine(m_currentIndex);
        
        if (endOfLineInfo.index != std::string::npos)
        {
            m_currentIndex = endOfLineInfo.index + endOfLineInfo.terminatorOffset;
        } 
        else 
        {
            m_currentIndex = m_source.size();
        }
    }

    std::vector<Token> Tokenize() 
    {
        std::vector<Token> tokens;
        
        std::string line;
        uint32_t lineIndex = 1;
        
        while (TryPeek(line))
        {
            const static std::regex TitleRgx { "(#{1,6}) (.*)" };
            const static std::regex BlockquoteRgx { "(>{1} )(.*)" };
            // Note : Maybe check for a more efficient regex
            const static std::regex NotEmptyLineRgx { "(?!( {1,}|\n{1,}|\r{1,}|$))" };

            std::smatch match;

            if(line.size() == 0 || !std::regex_search(line, NotEmptyLineRgx))
            {
                Consume();

                tokens.push_back({
                    .type = TokenType::EmptyLine,
                    .ln = lineIndex,
                    .col = 1U,
                });
            }
            else if (std::regex_match(line, match, BlockquoteRgx))
            {
                Consume();

                tokens.push_back({
                    .type = TokenType::QuoteLine,
                    .value = match[2],
                    .ln = lineIndex,
                    .col = static_cast<uint32_t>(match.position(2)) + 1,
                });
            }
            else if (std::regex_match(line, match, TitleRgx))
            {
                Consume();

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
                Consume();

                tokens.push_back({
                    .type = TokenType::TextLine,
                    .value = line,
                    .ln = lineIndex,
                    .col = 1U,
                });
            }

            ++lineIndex;
        }

        return tokens;
    }

private:

    struct EndOfLineInfo { size_t index = 0; uint8_t terminatorOffset = 0; };
    EndOfLineInfo FindEndOfLine(size_t startPos = 0) const
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

    const std::string& m_source;
    size_t m_currentIndex = 0;
};
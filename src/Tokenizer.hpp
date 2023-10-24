#include <vector>
#include <regex>

enum class TokenType
{
    TextLine,           // A line not Tokenized as an other token (excluding empty line)
    TextLiteral,        // Any sequence of characters except end of line
    TitleSymbol,        // `#{1,6}` usually followed by the name of the note as a `TextLiteral` (exemple : `# My super duper note`)
};

struct Token
{
    TokenType type;
    std::string value;
};

class Tokenizer 
{
public:
    Tokenizer(const std::string& source) 
        : m_source(source)
        , m_currentIndex(0) 
    {}

    bool TryPeek(std::string& line) 
    {
        if (m_currentIndex >= m_source.size())
        {
            return false;
        }

        size_t lineEndIndex = FindEndOfLineIndex(m_currentIndex);

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
        size_t lineEnd = m_source.find('\n', m_currentIndex);
        
        if (lineEnd != std::string::npos)
        {
            m_currentIndex = lineEnd + 1;
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
        while (TryPeek(line)) 
        {
            const static std::regex TitleRgx { "(#{1,6}) (.*)" };
            // Note : Maybe check for a more efficient regex
            const static std::regex NotEmptyLineRgx { "(?!( {1,}|\n{1,}|\r{1,}|$))" };

            std::smatch match;

            if(!std::regex_search(line, NotEmptyLineRgx))
            {
                Consume();
            }
            else if (std::regex_match(line, match, TitleRgx))
            {
                Consume();

                tokens.push_back({
                    TokenType::TitleSymbol,
                    match[1]
                });

                tokens.push_back({
                    TokenType::TextLiteral,
                    match[2]
                });
            }
            else
            {
                Consume();

                tokens.push_back({
                    TokenType::TextLiteral,
                    line
                });
            }
        }

        return tokens;
    }

private:

    size_t FindEndOfLineIndex(size_t startPos = 0) const
    {
        size_t length = m_source.length();
        
        for (size_t i = startPos; i < length; ++i) 
        {
            char character = m_source[i];

            if (character == '\n' || character == '\r') 
                return i;
        }
        
        return std::string::npos;
    }

    const std::string& m_source;
    size_t m_currentIndex = 0;
};
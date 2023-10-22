#include <vector>
#include <regex>

enum class TokenType
{
    TextLiteral,        // Any chunk of text not Tokenized as an other token
    TitleSymbol,        // `# ` usually followed by the name of the note as a `TextLiteral` (exemple : `# My super duper note`)
    SubTitleSymbol      // `## ` followed by the name of the sub-title (exemple : `## My super sub title`)
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

        // TODO : Make this cross format (accept '\n' '\r' '\n\r')
        size_t lineEnd = m_source.find('\n', m_currentIndex);
     
        if (lineEnd != std::string::npos) 
        {
            line = m_source.substr(m_currentIndex, lineEnd - m_currentIndex);
        } 
        else 
        {
            // This handles the last line (no more '\n')
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
            Token token = MatchPattern(line);
            tokens.push_back(token);
        }

        return tokens;
    }

private:
    Token MatchPattern(const std::string& line) 
    {
        const static std::regex TitleSymbolRgx                  { "^#\\s(.*)" };
        const static std::regex SubTitleSymbolRgx               { "^##\\s(.*)" };

        std::smatch match;

        if (std::regex_match(line, match, TitleSymbolRgx)) 
        {
            Consume();

            return { 
                TokenType::TitleSymbol,
                match[1] 
            };
        } 
        else if (std::regex_match(line, match, SubTitleSymbolRgx)) 
        {
            Consume();

            return {
                TokenType::SubTitleSymbol,
                match[1]
            };
        } 
        else 
        {
            Consume();

            return { 
                TokenType::TextLiteral,
                line 
            };
        }
    }

    const std::string& m_source;
    size_t m_currentIndex = 0;
};
#include <vector>

#include "Tokenizer.hpp"

class Parser
{
public:
    Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens))
    {
        
    }

    // std::vector<> ParseProgram()
    // {

    // }

private:
    std::vector<Token> m_tokens;
};
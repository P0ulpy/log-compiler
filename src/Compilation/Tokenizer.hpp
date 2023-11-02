#pragma once

#include <vector>

#include "Token.hpp"

class Tokenizer
{
public:
    Tokenizer() = delete;
    Tokenizer(std::string source);
    
    std::vector<Token> Tokenize();

private:
    [[nodiscard]] bool TryPeek(std::string& line) const;
    void Consume();

    struct EndOfLineInfo { size_t index = 0; uint16_t terminatorOffset = 0; };
    [[nodiscard]] EndOfLineInfo FindEndOfLine(size_t startPos = 0) const;
    
private:
    std::string m_source;
    size_t m_cursor = 0;
};
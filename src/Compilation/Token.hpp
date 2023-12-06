#pragma once

#include <string>
#include <ostream>

/** TODO :
 * Add end of line concept of handle `<br>` and `\` at end of a text line 
 * maybe the concept of Texline will be deleted and we will keep only TextLiteral and endline
*/
enum class TokenType
{
    EmptyLine,          // An empty line containing only one or more `\s`, `\n` or `\r` characters
    TextLine,           // A not empty line not Tokenized as an other token
    TextLiteral,        // Any sequence of characters except end of line
    TitleSymbol,        // `#{1,6}` usually followed by the name of the note as a `TextLiteral` (exemple : `# My super duper note`)
    QuoteBlockLine,     // `> ` followed by the content of the QuoteLine (exemple : `> My super duper quoteblock line`)
};

const char* TokenTypeToCstr(TokenType tokenType);

struct Token
{
    TokenType type;
    std::string value;
    uint32_t ln = 0, col = 0;
};

std::ostream& operator<<(std::ostream& os, const Token& token);
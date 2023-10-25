#include <vector>
#include <variant>

#include "TypeList.hpp"
#include "Tokenizer.hpp"

struct TextLineToken
{
    std::string content;
};

struct TitleToken
{
    // Title of level 0 is invalid, assert this at Parsing (should be impossible but throw an error anyway to tell Tokanization messed up)
    uint8_t level = 0;
    std::string content;
};

struct QuoteBlockToken
{
    // QuoteBlock with 0 elements is invalid, assert this at Parsing (should be impossible but throw an error anyway to tell Tokanization messed up)    
    std::vector<std::string> content;
};

using TokensTypes = TypeList<
    std::unique_ptr<TextLineToken>,
    std::unique_ptr<TitleToken>,
    std::unique_ptr<QuoteBlockToken>
>;

using ProgramTokenVariant = TypeListToVariant<TokensTypes>::type;

// NOTE :   To improve the memory continguity of the Pasing phase we could maybe use an ArenaAllocator with a computed size at Parser construction
//          this whould prevent from making a heap allocations for every tokens 

class Parser
{
public:
    Parser(const std::vector<Token>& tokens)
        : m_tokens(tokens)
    {
        
    }

    std::vector<ProgramTokenVariant> ParseProgram()
    {
        std::vector<ProgramTokenVariant> program;

        return program;
    }

private:
    std::vector<Token> m_tokens;
};
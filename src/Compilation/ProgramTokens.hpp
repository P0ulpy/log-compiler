#pragma once

#include <vector>
#include <variant>
#include <ostream>

#include "../Utils/TypeList.hpp"
#include "../Utils/StringUtils.hpp"

using TextLine = std::string;

struct TextBlockToken
{
    std::vector<TextLine> lines;
};

std::ostream& operator<<(std::ostream& os, const TextBlockToken& token);

// Note : TitleToken is the only token who can not be inside a NodeContent.
// It should not be add to ProgramTokensTypes TypeList, this would cause crash in Visitors type checking 
struct TitleToken
{
    uint16_t level = 0;
    TextLine text;
};

std::ostream& operator<<(std::ostream& os, const TitleToken& token);

struct QuoteBlockToken
{
    std::vector<TextLine> lines;
};

std::ostream& operator<<(std::ostream& os, const QuoteBlockToken& token);

struct NodeToken;

/** Tokens Types declaration
 * All Tokens types MUST be declared in the TypeList, 
 * otherwise they will not be processed correctly in rest of the systems
*/
using ProgramTokensTypes = TypeList<
    TextBlockToken,
    QuoteBlockToken,
    NodeToken
>;

using ProgramTokenVariant = ProgramTokensTypes::ToVariant_t;

using NodeContent = std::vector<ProgramTokenVariant>;

struct NodeToken
{
    TitleToken title;
    NodeContent content;
};

struct ProgramRoot
{
    std::string name;
    NodeContent content;
};

std::ostream& operator<<(std::ostream& os, const NodeToken& token);
#pragma once

#include <vector>
#include <variant>
#include <ostream>

#include "../Utils/TypeList.hpp"
#include "../Utils/StringUtils.hpp"

struct TextBlockToken
{
    std::string text;
};

std::ostream& operator<<(std::ostream& os, const TextBlockToken& token);

struct TitleToken
{
    uint16_t level = 0;
    std::string text;
};

std::ostream& operator<<(std::ostream& os, const TitleToken& token);

struct QuoteBlockToken
{
    std::string text;
};

std::ostream& operator<<(std::ostream& os, const QuoteBlockToken& token);

struct NodeToken;

/** Tokens Types declaration
 * All Tokens types MUST be declared in the TypeList, 
 * otherwise it will not be processed correctly in rest of the systems
*/
using ProgramTokensTypes = TypeList<
    TextBlockToken,
    TitleToken,
    QuoteBlockToken,
    NodeToken
>;

using ProgramTokenVariant = ProgramTokensTypes::ToVariant_t;

struct NodeToken
{
    TitleToken title;
    std::vector<ProgramTokenVariant> content;
};

struct ProgramRoot
{
    std::string name;
    std::vector<ProgramTokenVariant> content;
};

std::ostream& operator<<(std::ostream& os, const NodeToken& token);
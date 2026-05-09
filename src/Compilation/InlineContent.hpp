#pragma once

#include <string>
#include <vector>
#include <variant>
#include <ostream>

struct PlainText 
{
    std::string text;
};

struct LinkInline 
{
    std::string label;
    std::string url;
};

using InlineElement = std::variant<PlainText, LinkInline>;
using RichTextLine = std::vector<InlineElement>;

std::ostream& operator<<(std::ostream& os, const PlainText& text);
std::ostream& operator<<(std::ostream& os, const LinkInline& link);
std::ostream& operator<<(std::ostream& os, const RichTextLine& line);

RichTextLine ParseInlineContent(const std::string& raw);
std::string UnescapeText(const std::string& raw);

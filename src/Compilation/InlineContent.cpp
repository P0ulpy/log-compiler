#include "InlineContent.hpp"

#include <sstream>

std::ostream& operator<<(std::ostream& os, const PlainText& text)
{
    os << text.text;
    return os;
}

std::ostream& operator<<(std::ostream& os, const LinkInline& link)
{
    os << "[" << link.label << "](" << link.url << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const RichTextLine& line)
{
    for (const auto& elem : line)
    {
        std::visit([&os](auto&& e) { os << e; }, elem);
    }
    return os;
}

std::string UnescapeText(const std::string& raw)
{
    std::string result;
    result.reserve(raw.size());

    for (size_t i = 0; i < raw.size(); ++i)
    {
        if (raw[i] == '\\' && (i + 1) < raw.size())
        {
            char next = raw[i + 1];
            // Escape special characters: #, >, \, [, ], (, )
            if (next == '#' || next == '>' || next == '\\' || 
                next == '[' || next == ']' || next == '(' || next == ')')
            {
                result += next;
                ++i;
                continue;
            }
        }
        result += raw[i];
    }

    return result;
}

RichTextLine ParseInlineContent(const std::string& raw)
{
    RichTextLine elements;
    std::string currentText;

    for (size_t i = 0; i < raw.size(); ++i)
    {
        // Handle escape character
        if (raw[i] == '\\' && (i + 1) < raw.size())
        {
            char next = raw[i + 1];
            if (next == '[' || next == ']' || next == '(' || next == ')' || next == '\\')
            {
                currentText += next;
                ++i;
                continue;
            }
        }

        // Try to match [label](url)
        if (raw[i] == '[')
        {
            size_t labelStart = i + 1;
            size_t labelEnd = std::string::npos;

            // Find closing ]
            for (size_t j = labelStart; j < raw.size(); ++j)
            {
                if (raw[j] == '\\' && (j + 1) < raw.size())
                {
                    ++j;
                    continue;
                }
                if (raw[j] == ']')
                {
                    labelEnd = j;
                    break;
                }
            }

            if (labelEnd != std::string::npos && (labelEnd + 1) < raw.size() && raw[labelEnd + 1] == '(')
            {
                size_t urlStart = labelEnd + 2;
                size_t urlEnd = std::string::npos;

                // Find closing )
                for (size_t j = urlStart; j < raw.size(); ++j)
                {
                    if (raw[j] == '\\' && (j + 1) < raw.size())
                    {
                        ++j;
                        continue;
                    }
                    if (raw[j] == ')')
                    {
                        urlEnd = j;
                        break;
                    }
                }

                if (urlEnd != std::string::npos)
                {
                    // Flush accumulated plain text
                    if (!currentText.empty())
                    {
                        elements.push_back(PlainText{ currentText });
                        currentText.clear();
                    }

                    std::string label = raw.substr(labelStart, labelEnd - labelStart);
                    std::string url = raw.substr(urlStart, urlEnd - urlStart);
                    elements.push_back(LinkInline{ UnescapeText(label), UnescapeText(url) });

                    i = urlEnd;
                    continue;
                }
            }
        }

        currentText += raw[i];
    }

    // Flush remaining text
    if (!currentText.empty())
    {
        elements.push_back(PlainText{ currentText });
    }

    return elements;
}

#include <iostream>
#include <cstdint>

#include "MdGenerator.hpp"

MdGenerator::MdGenerator(ProgramRoot& program)
    : Generator(program)
{}

const std::stringstream &MdGenerator::Generate()
{
    ProcessProgramContent(m_program.content);   
    return m_output;
}

void MdGenerator::ProcessProgramContent(const NodeContent &content)
{
    MdGeneratorVisitor tokenVisitor(*this);

    for(size_t tokenIndex = 0; tokenIndex < content.size(); tokenIndex++)
    {
        auto& tokenVariant = content[tokenIndex];

        std::visit([&](auto&& token)
        {
            tokenVisitor(token);

        }, tokenVariant);
    }
}

void MdGenerator::Append(const std::string_view &value)
{
    m_output << value;
}

void MdGenerator::AppendLine(const std::string_view &value)
{
    m_output << value << '\n';
}

void MdGenerator::AppendBlock(const std::string_view &value)
{
    m_output << value << "\n\n";
}

void MdGenerator::WriteInlineLine(const RichTextLine& line)
{
    for (const auto& elem : line)
    {
        std::visit([this](auto&& e)
        {
            using Type = std::decay_t<decltype(e)>;
            if constexpr (std::is_same_v<Type, PlainText>)
            {
                m_output << EscapePlainText(e.text);
            }
            else if constexpr (std::is_same_v<Type, LinkInline>)
            {
                m_output << "[" << EscapeLinkLabel(e.label) << "](" << EscapeLinkUrl(e.url) << ")";
            }
        }, elem);
    }
}

std::string MdGenerator::EscapePlainText(const std::string& text)
{
    std::string result;
    result.reserve(text.size());

    for (char c : text)
    {
        if (c == '[' || c == ']')
            result += '\\';
        result += c;
    }

    return result;
}

std::string MdGenerator::EscapeLinkLabel(const std::string& label)
{
    std::string result;
    result.reserve(label.size());

    for (char c : label)
    {
        if (c == ']' || c == '\\' || c == '[')
            result += '\\';
        result += c;
    }

    return result;
}

std::string MdGenerator::EscapeLinkUrl(const std::string& url)
{
    std::string result;
    result.reserve(url.size());

    for (char c : url)
    {
        if (c == ')' || c == '\\' || c == '(')
            result += '\\';
        result += c;
    }

    return result;
}

MdGeneratorVisitor::MdGeneratorVisitor(MdGenerator& generator)
    : gen(generator)
{}

template <>
void MdGeneratorVisitor::operator()(const NodeToken &token)
{
    // Title prefix
    uint16_t level = token.title.level;
    std::string title;
    for(uint16_t i = 0; i < level; ++i) title += '#';

    title += ' ' + token.title.text;
    gen.AppendLine(title);
    gen.ProcessProgramContent(token.content);
}

template <>
void MdGeneratorVisitor::operator()(const TextBlockToken &value)
{
    for (const auto& line : value.lines)
    {
        gen.WriteInlineLine(line);
        gen.m_output << '\n';
    }

    gen.Append("\n");
}

template <>
void MdGeneratorVisitor::operator()(const QuoteBlockToken &value)
{
    for (const auto& line : value.lines)
    {
        gen.m_output << "> ";
        gen.WriteInlineLine(line);
        gen.m_output << '\n';
    }

    gen.Append("\n");
}

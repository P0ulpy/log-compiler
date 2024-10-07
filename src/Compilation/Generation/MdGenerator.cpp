#include <iostream>

#include "MdGenerator.hpp"

MdGenerator::MdGenerator(ProgramRoot& program)
    : Generator(program)
{}

const std::stringstream &MdGenerator::Generate()
{
    ProcesssProgramContent(m_program.content);   
    return m_output;
}

void MdGenerator::ProcesssProgramContent(const NodeContent &content)
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
    gen.ProcesssProgramContent(token.content);
}

template <>
void MdGeneratorVisitor::operator()(const TextBlockToken &value)
{
    for (const auto& line : value.lines)
    {
        gen.AppendLine(line);
    }

    gen.Append("\n");
}

template <>
void MdGeneratorVisitor::operator()(const QuoteBlockToken &value)
{
    for (const auto& line : value.lines)
    {
        std::string blockquoteLine = "> " + line;
        gen.AppendLine(blockquoteLine);
    }

    gen.Append("\n");
}

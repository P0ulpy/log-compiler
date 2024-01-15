#include <iostream>

#include "MdGenerator.hpp"

MdGenerator::MdGenerator(ProgramRoot program)
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
    gen.AppendBlock(value.text);
}

template <>
void MdGeneratorVisitor::operator()(const QuoteBlockToken &value)
{
    using namespace std::literals;

    // NOTE : Will be replaced with a lines enumeration when QuoteBlockToken type will evolve (cf: ProgramTokens.hpp TextBlockToken comentary)
    auto text = value.text;

    std::stringstream content;
    content << text;

    std::string line;
    for (std::string line; std::getline(content, line);)
    {
        std::string blockquoteLine = "> "s + line;
        gen.AppendLine(blockquoteLine);
    }

    gen.Append("\n");
}

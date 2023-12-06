#include "JsonGenerator.hpp"

#include <iostream>

JsonGenerator::JsonGenerator(ProgramRoot program)
    : Generator(program)
{}

const std::stringstream &JsonGenerator::Generate()
{
    BeginObject();

        Field("name");
        LiteralString(m_program.name);

        Field("content");
        BeginTab();
            ProcessProgramContent(m_program.content);
        EndTab(false);

    EndObject(false);

    return m_output;
}

void JsonGenerator::ProcessProgramContent(const NodeContent& content)
{
    JsonGeneratorVisitor tokenVisitor(*this);

    for(size_t tokenIndex = 0; tokenIndex < content.size(); tokenIndex++)
    {
        auto& tokenVariant = content[tokenIndex];
        bool isLast = (tokenIndex + 1) >= content.size();

        std::visit([&](auto&& token)
        {
            tokenVisitor(token, isLast);

        }, tokenVariant);
    }
}

inline void JsonGenerator::Field(const std::string_view &label)
{
    m_output << '"' << label << "\":";
}

void JsonGenerator::LiteralString(const std::string_view& value, bool comma)
{
    m_output << '\"' << FormatToJsonString(value) << '\"';
    HasComma(comma);
}

void JsonGenerator::LiteralInteger(const int64_t& value, bool comma)
{
    m_output << FormatToJsonString(std::to_string(value));
    HasComma(comma);
}

void JsonGenerator::LiteralFloating(double value, bool comma)
{
    m_output << FormatToJsonString(std::to_string(value));
    HasComma(comma);
}

void JsonGenerator::LiteralBoolean(bool value, bool comma)
{
    m_output << FormatToJsonString(std::to_string(value));
    HasComma(comma);
}

JsonGeneratorVisitor::JsonGeneratorVisitor(JsonGenerator& generator)
    : gen(generator)
{}

template <>
void JsonGeneratorVisitor::operator()(const NodeToken &token, bool isLast)
{
    gen.BeginObject();

        gen.Field("type");
        gen.LiteralString("section");

        gen.Field("title");
        gen.LiteralString(token.title.text);
        gen.Field("level");
        gen.LiteralInteger(token.title.level);

        gen.Field("content");
        gen.BeginTab();
            gen.ProcessProgramContent(token.content);
        gen.EndTab(false);

    gen.EndObject(!isLast);
}

template <>
void JsonGeneratorVisitor::operator()(const TextBlockToken &value, bool isLast)
{
    gen.BeginObject();

        gen.Field("type");
        gen.LiteralString("plain-text");
        gen.Field("text");
        gen.LiteralString(value.text, false);

    gen.EndObject(!isLast);
}

template <>
void JsonGeneratorVisitor::operator()(const QuoteBlockToken &value, bool isLast)
{
    gen.BeginObject();

        gen.Field("type");
        gen.LiteralString("block-quotes");
        gen.Field("text");
        gen.LiteralString(value.text, false);

    gen.EndObject(!isLast);
}

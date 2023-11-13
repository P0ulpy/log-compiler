#include "JsonGenerator.hpp"

#include <iostream>

JsonGenerator::JsonGenerator(ProgramRoot program)
    : Generator(program)
{}

const std::stringstream &JsonGenerator::Generate()
{
    BeginObject();

        Field("name");
        Literal<String>(m_program.name);

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

template <>
void JsonGenerator::Literal<JsonGenerator::LiteralType::String>(const std::string_view &value, bool comma)
{
    m_output << '\"' << FormatToJsonString(value) << '\"';
    HasComma(comma);
}

template <>
void JsonGenerator::Literal<JsonGenerator::LiteralType::Integer>(const std::string_view &value, bool comma)
{
    throw std::runtime_error("Not implemented");
}

template <>
void JsonGenerator::Literal<JsonGenerator::LiteralType::Float>(const std::string_view &value, bool comma)
{
    throw std::runtime_error("Not implemented");
}

template <>
void JsonGenerator::Literal<JsonGenerator::LiteralType::Bool>(const std::string_view &value, bool comma)
{
    throw std::runtime_error("Not implemented");
}



JsonGeneratorVisitor::JsonGeneratorVisitor(JsonGenerator& generator)
    : gen(generator)
{}

template <>
void JsonGeneratorVisitor::operator()(const NodeToken &token, bool isLast)
{
    gen.BeginObject();
        gen.Field("title");
        gen.Literal<JsonGenerator::String>(token.title.text);
        // gen.Field("level");
        // gen.Literal<JsonGenerator::Integer>(token.title.level);
                            
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
        gen.Literal<JsonGenerator::String>("plain-text");
        gen.Field("text");
        gen.Literal<JsonGenerator::String>(value.text, false);

    gen.EndObject(!isLast);
}

template <>
void JsonGeneratorVisitor::operator()(const QuoteBlockToken &value, bool isLast)
{
    gen.BeginObject();

        gen.Field("type");
        gen.Literal<JsonGenerator::String>("block-quotes");
        gen.Field("text");
        gen.Literal<JsonGenerator::String>(value.text, false);

    gen.EndObject(!isLast);
}

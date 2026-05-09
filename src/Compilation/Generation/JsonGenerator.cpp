#include "JsonGenerator.hpp"

#include <iostream>

JsonGenerator::JsonGenerator(ProgramRoot& program)
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

void JsonGenerator::WriteInlineElements(const RichTextLine& line)
{
    for (size_t i = 0; i < line.size(); ++i)
    {
        bool elemLast = (i + 1) >= line.size();

        std::visit([&](auto&& elem)
        {
            using Type = std::decay_t<decltype(elem)>;
            if constexpr (std::is_same_v<Type, PlainText>)
            {
                BeginObject();
                    Field("type"); LiteralString("text");
                    Field("value"); LiteralString(elem.text, false);
                EndObject(!elemLast);
            }
            else if constexpr (std::is_same_v<Type, LinkInline>)
            {
                BeginObject();
                    Field("type"); LiteralString("link");
                    Field("label"); LiteralString(elem.label);
                    Field("url"); LiteralString(elem.url, false);
                EndObject(!elemLast);
            }
        }, line[i]);
    }
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

        gen.Field("type"); gen.LiteralString("plain-text");

        gen.Field("lines");         
        gen.BeginTab();

        for(size_t i = 0; i < value.lines.size(); ++i)
        {
            gen.BeginObject();
                gen.Field("elements");
                gen.BeginTab();
                    gen.WriteInlineElements(value.lines[i]);
                gen.EndTab(false);
            gen.EndObject(i + 1 < value.lines.size());
        }

        gen.EndTab(false);

    gen.EndObject(!isLast);
}

template <>
void JsonGeneratorVisitor::operator()(const QuoteBlockToken &value, bool isLast)
{
    gen.BeginObject();

        gen.Field("type");
        gen.LiteralString("block-quotes");
        
        gen.Field("lines"); 
        gen.BeginTab();

        for(size_t i = 0; i < value.lines.size(); ++i)
        {
            gen.BeginObject();
                gen.Field("elements");
                gen.BeginTab();
                    gen.WriteInlineElements(value.lines[i]);
                gen.EndTab(false);
            gen.EndObject(i + 1 < value.lines.size());
        }

        gen.EndTab(false);

    gen.EndObject(!isLast);
}

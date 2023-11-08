#include "JsonGenerator.hpp"

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


    EndObject();

    return m_output;
}

void JsonGenerator::ProcessProgramContent(const decltype(ProgramRoot::content)& content)
{
    for(size_t tokenIndex = 0; tokenIndex < content.size(); tokenIndex++)
    {
        auto& tokenVariant = content[tokenIndex];
        bool isNotLast = ((tokenIndex + 1) >= content.size());

        std::visit([&](auto&& token) 
        {
            using Type = std::decay_t<decltype(token)>;

            if constexpr (std::is_same_v<NodeToken, Type>) 
            {
                const auto& nodetoken = static_cast<const NodeToken&>(token);
                m_output << '"' << nodetoken.title << '"';
                HasComma(isNotLast);
                ProcessProgramContent(nodetoken.content);
            }
            else
            {
                m_output << '"' << token << '"';
                HasComma(isNotLast);
            }
        
        }, tokenVariant);
    }
}

inline void JsonGenerator::Field(const std::string_view &label)
{
    m_output << '"' << label << "\":";
}

template <>
void JsonGenerator::Literal<JsonGenerator::LiteralType::String>(const std::string_view &value, bool noComma)
{
    m_output << '\"' << value << '\"';
    HasComma(noComma);
}

template <>
void JsonGenerator::Literal<JsonGenerator::LiteralType::Integer>(const std::string_view &value, bool noComma)
{
    throw std::runtime_error("Not implemented");
}

template <>
void JsonGenerator::Literal<JsonGenerator::LiteralType::Float>(const std::string_view &value, bool noComma)
{
    throw std::runtime_error("Not implemented");
}
#pragma once

#include <string_view>

#include "Generator.hpp"

class JsonGenerator : public Generator
{
public:
    enum LiteralType { String, Integer, Float, Bool };

public:
    JsonGenerator(ProgramRoot program);

    const char* GetFileExtention() const final { return ".json"; }

    const std::stringstream& Generate() final;

private:
    void ProcessProgramContent(const decltype(ProgramRoot::content)& content);

    void BeginObject()                          { m_output << '{'; }
    void EndObject(bool comma = true)           { m_output << '}'; HasComma(comma); }

    void BeginTab()                             { m_output << '['; }
    void EndTab(bool comma = true)              { m_output << ']'; HasComma(comma); }

    void HasComma(bool comma)                   { m_output << ((comma) ? "," : ""); }

    void Field(const std::string_view& label);

    template <JsonGenerator::LiteralType type>
    void Literal(const std::string_view &value, bool comma = true) = delete;
};

template <> void JsonGenerator::Literal<JsonGenerator::LiteralType::String>(const std::string_view& value, bool comma);
template <> void JsonGenerator::Literal<JsonGenerator::LiteralType::Integer>(const std::string_view& value, bool comma);
template <> void JsonGenerator::Literal<JsonGenerator::LiteralType::Float>(const std::string_view& value, bool comma);
template <> void JsonGenerator::Literal<JsonGenerator::LiteralType::Bool>(const std::string_view& value, bool comma);
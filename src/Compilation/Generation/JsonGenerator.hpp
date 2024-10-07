#pragma once

#include <string_view>
#include <concepts>

#include "Generator.hpp"
#include "../../Utils/Assertion.hpp"

class JsonGeneratorVisitor;

class JsonGenerator : public Generator
{
public:
    enum LiteralType { String, Integer, Float, Bool };

public:
    JsonGenerator(ProgramRoot& program);

    const char* GetFileExtention() const final { return ".json"; }

    const std::stringstream& Generate() final;

private:
    void ProcessProgramContent(const NodeContent& content);

    void BeginObject()                          { m_output << '{'; }
    void EndObject(bool comma = true)           { m_output << '}'; HasComma(comma); }

    void BeginTab()                             { m_output << '['; }
    void EndTab(bool comma = true)              { m_output << ']'; HasComma(comma); }

    void HasComma(bool comma)                   { m_output << ((comma) ? "," : ""); }

    void Field(const std::string_view& label);

    void LiteralString(const std::string_view& value, bool comma = true);
    void LiteralInteger(const int64_t& value, bool comma = true);
    void LiteralFloating(double value, bool comma = true);
    void LiteralBoolean(bool value, bool comma = true);

    friend class JsonGeneratorVisitor;
};

class JsonGeneratorVisitor
{
public:
    JsonGeneratorVisitor() = delete;
    JsonGeneratorVisitor(JsonGenerator& generator);

    template<typename TToken>
    void operator()(const TToken& value, bool isLast);

private:
    JsonGenerator& gen;
};

template <> void JsonGeneratorVisitor::operator()<NodeToken>(const NodeToken& value, bool isLast);
template <> void JsonGeneratorVisitor::operator()<TextBlockToken>(const TextBlockToken& value, bool isLast);
template <> void JsonGeneratorVisitor::operator()<QuoteBlockToken>(const QuoteBlockToken& value, bool isLast);

template <class TToken>
inline void JsonGeneratorVisitor::operator()(const TToken &value, bool isLast)
{
    static_assert(assert_false_v<TToken>, "Unhandled Token Type, please add a visitor specialisation for this type");
}
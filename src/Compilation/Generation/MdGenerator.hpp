#pragma once

#include <string_view>
#include <concepts>

#include "Generator.hpp"
#include "../../Utils/Assertion.hpp"

class MdGeneratorVisitor;

class MdGenerator : public Generator
{
public:
    MdGenerator(ProgramRoot program);

    const char* GetFileExtention() const final { return ".md"; }

    const std::stringstream& Generate() final;

private: 
    void ProcesssProgramContent(const NodeContent& content);
    void Append(const std::string_view& value);
    void AppendLine(const std::string_view& value);
    void AppendBlock(const std::string_view& value);

private:
    friend class MdGeneratorVisitor;
};

class MdGeneratorVisitor
{
public:
    MdGeneratorVisitor() = delete;
    MdGeneratorVisitor(MdGenerator& generator);

    template<typename TToken>
    void operator()(const TToken& value);

private:
    MdGenerator& gen;
};

template <> void MdGeneratorVisitor::operator()<NodeToken>(const NodeToken& value);
template <> void MdGeneratorVisitor::operator()<TextBlockToken>(const TextBlockToken& value);
template <> void MdGeneratorVisitor::operator()<QuoteBlockToken>(const QuoteBlockToken& value);

template <class TToken>
inline void MdGeneratorVisitor::operator()(const TToken &value)
{
    static_assert(assert_false_v<TToken>, "Unhandled Token Type, please add a visitor specialisation for this type");
}
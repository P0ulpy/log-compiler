#pragma once

#include <string_view>
#include <concepts>

#include "Generator.hpp"

class MdGeneratorVisitor;

class MdGenerator : public Generator
{
public:
    MdGenerator(ProgramRoot program);

    const char* GetFileExtention() const final { return ".md"; }

    const std::stringstream& Generate() final;

private:
    friend class JsonGeneratorVisitor;
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
    throw std::runtime_error(std::string("Unhandled Token Type, please add a visitor specialisation for this type ") + typeid(value).name());
}
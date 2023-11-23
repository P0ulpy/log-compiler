#include "MdGenerator.hpp"

#include <iostream>

MdGenerator::MdGenerator(ProgramRoot program)
    : Generator(program)
{}

const std::stringstream &MdGenerator::Generate()
{
    return m_output;
}

MdGeneratorVisitor::MdGeneratorVisitor(MdGenerator& generator)
    : gen(generator)
{}

template <>
void MdGeneratorVisitor::operator()(const NodeToken &token)
{
}

template <>
void MdGeneratorVisitor::operator()(const TextBlockToken &value)
{
}

template <>
void MdGeneratorVisitor::operator()(const QuoteBlockToken &value)
{
}

#pragma once

#include "Generator.hpp"

class JsonGenerator : public Generator
{
public:
    JsonGenerator(ProgramRoot program)
        : Generator(program)
    {}

    const std::stringstream& Generate() final
    {
        m_output << "{}";

        return m_output;
    }
};
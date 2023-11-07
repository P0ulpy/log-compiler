#pragma once

#include "Generator.hpp"

class JsonGenerator : public Generator
{
public:
    const std::stringstream& Generate() final
    {

        return m_output;
    }
};
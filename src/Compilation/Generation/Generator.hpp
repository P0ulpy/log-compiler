#pragma once

#include <sstream>

#include "../ProgramTokens.hpp"


class Generator 
{
public:
    Generator(ProgramRoot program)
        : m_program(std::move(program))
    {}

    virtual const char* GetFileExtention() const { return ".out"; };
    virtual const std::stringstream& Generate() = 0;

protected:
    std::stringstream m_output;
    ProgramRoot m_program;
};
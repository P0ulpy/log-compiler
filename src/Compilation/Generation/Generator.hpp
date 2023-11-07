#pragma once

#include <sstream>

#include "../ProgramTokens.hpp"


class Generator 
{
public:
    Generator() = delete;
    virtual ~Generator() = 0;

    Generator(ProgramRoot program)
        : m_program(std::move(program))
    {}

    virtual const std::stringstream& Generate() = 0;

protected:
    std::stringstream m_output;
    ProgramRoot m_program;
};
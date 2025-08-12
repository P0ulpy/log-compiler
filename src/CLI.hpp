#pragma once

#include <iostream>

#include "main.hpp"
#include "Compilation/Token.hpp"
#include "Compilation/ProgramTokens.hpp"

const char* OutputFormatsToCstr(CompilerOptions::OutputFormats tokenType);

constexpr const char* ShortCLIUsageStr = "Usage : log-compiler InputFile [--output outputFile] [--format json|(markdown|md)|c] [--render] [--version] [--help] [--verbose] [--debug]";

constexpr const char* CompleteCLIUsageStr =
R"(positional arguments:
  InputFile         Specifies the input log file to be compiled.

optional arguments:
  -o, --output      Specifies the output file path (default=./a.{format}).
  -f, --format      Specifies the output format as `JSON`, `Markdown` or `C + RayGui` format : `[json|(markdown|md)|c]` (default=json).
  -v, --version     display compiler version, then exit.
  --verbose         Enables verbose mode for detailed output.
  -d, --debug       Enables debug mode for debugging information.
  -h, --help        display this message, then exit.
)";

void StdOutInputFileInfo(const std::string_view& inputFilePath, const std::stringstream& fileContent);
void StdOutTokens(const std::vector<Token>& tokens);
void StdOutProgram(const ProgramRoot& program);

struct ParsedCLIParameters
{
    std::string inputFilePath;
    std::string outputFilePath;
};

ParsedCLIParameters ProcessCLIArgs(int argc, const char** argv);
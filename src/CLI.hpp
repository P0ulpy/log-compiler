#pragma once

#include <iostream>

#include "main.hpp"
#include "InputsParser.hpp"
#include "Tokenizer.hpp"

const char* TokenTypeToCstr(TokenType tokenType)
{
    switch(tokenType)
    {
        case TokenType::EmptyLine :         return "EmptyLine";
        case TokenType::TextLine :          return "TextLine";
        case TokenType::TextLiteral :       return "TextLiteral";
        case TokenType::TitleSymbol :       return "TitleSymbol";
        case TokenType::QuoteLine :         return "QuoteLine";
    }
}

const char* OutputFormatsToCstr(CompilerOptions::OutputFormats tokenType)
{
    using OutputFormats = CompilerOptions::OutputFormats;

    switch(tokenType)
    {
        case OutputFormats::JSON :      return "JSON";
        case OutputFormats::Markdown :  return "Markdown";
        case OutputFormats::C :         return "C";
    }
}

static const char* ShortCLIUsageStr = "Usage : log-compiler InputFile [-o outputFile] [--format json|(markdown|md)|c] [--verbose] [--debug] [-v] [-h]";

static const char* CompleteCLIUsageStr = 
R"(positional arguments:
  InputFile         Specifies the input log file to be compiled.

optional arguments:
  -o, --output      Specifies the output file path (optional, default=./a.{format}).
  --format          Specifies the output format as `JSON`, `Markdown` or `C + RayGui` format : `[json|(markdown|md)|c]` (optional, default=json).
  --verbose         Enables verbose mode for detailed output (optional).
  --debug           Enables debug mode for debugging information (optional).
  -v, --version     display compiler version, then exit.
  -h, --help        display this message, then exit.
)";

void StdOutInputFileInfo(const std::string_view& inputFilePath, const std::stringstream& fileContent)
{
    std::cout 
        << "Input file : " << inputFilePath << '\n'
        << std::endl;
}

void StdOutTokens(const std::vector<Token>& tokens)
{
    std::cout << "Tokens : \n";
    for(auto token : tokens)
    {
        std::cout 
            << "    { "
            << "Type: `" << TokenTypeToCstr(token.type) << '`'
            << ", Value: `" << token.value << '`' 
            << ", pos: " << token.ln << ':' << token.col
            << " }" 
            << std::endl;
    }
}

struct ParsedCLIParameters 
{
    std::string inputFilePath;
};

ParsedCLIParameters ProcessCLIArgs(const InputParser& inputParser)
{    
    if(inputParser.CmdOptionExists("--help") || inputParser.CmdOptionExists("-h"))
    {
        std::cout 
            << ShortCLIUsageStr
            << '\n' 
            << CompleteCLIUsageStr 
            << std::endl;
        exit(EXIT_SUCCESS);
    }

    if(inputParser.CmdOptionExists("--version") || inputParser.CmdOptionExists("-v"))
    {
        std::cout << "LogCompiler " << 'v' << VERSION_MAJOR << '.' << VERSION_MINOR << '.' << VERSION_PATCH << '-' << VERSION_TYPE << std::endl;
        exit(EXIT_SUCCESS);
    }

    // Note : Change the system to match positional kind of arguments not index
    if(!inputParser.CmdIndexExists(0))
    {
        std::cerr << "ERROR : No input file provided !\n" << ShortCLIUsageStr << std::endl;
        exit(EXIT_FAILURE);
    }

    ParsedCLIParameters params;
    params.inputFilePath = inputParser[0];

    if(inputParser.CmdOptionExists("--debug"))
        CompilerOptions::DebugMode = true;
    if(inputParser.CmdOptionExists("--verbose"))
        CompilerOptions::Verbose = true;

    if(CompilerOptions::DebugMode)
        std::cout << "You are un debug mode" << std::endl;

    if(inputParser.CmdOptionExists("--format"))
    {
        auto format = inputParser["--format"];
        std::transform(format.begin(), format.end(), format.begin(),
            [](unsigned char c){ return std::tolower(c); });
        
        if(format == "json")
            CompilerOptions::OutputFormat = CompilerOptions::OutputFormats::JSON;
        else if(format == "markdown" || format == "md")
            CompilerOptions::OutputFormat = CompilerOptions::OutputFormats::Markdown;
        else if(format == "c")
            CompilerOptions::OutputFormat = CompilerOptions::OutputFormats::C;
        else
        {
            std::cerr 
                << "ERROR : invalid --format `" << format << "` is not a valid format. "
                << "Accepted formats are [json|(markdown|md)|c]\n"
                << ShortCLIUsageStr << std::endl;
                
            exit(EXIT_FAILURE);
        }
    }

    if(CompilerOptions::Verbose)
    {
        std::cout
            << "Selected output format : `" << OutputFormatsToCstr(CompilerOptions::OutputFormat) << '`'
            << std::endl;
    }

    return params;
}
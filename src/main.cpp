#include <iostream>
#include <fstream>
#include <sstream>

#include "InputsParser.hpp"
#include "Tokenizer.hpp"

namespace CompilerOptions
{
    static bool DebugMode = false;
    static bool Verbose = false;

    enum class OutputFormats { JSON, Markdown, C };
    static OutputFormats OutputFormat = OutputFormats::JSON;
}

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


void StdOutInputFileInfo(const std::string_view& inputFilePath, const std::stringstream& fileContent)
{
    std::cout 
        << "For file : " << inputFilePath << '\n'
        << std::endl;
}

void StdOutTokens(const std::vector<Token>& tokens)
{
    std::cout << "Tokens : \n";
    for(auto token : tokens)
    {
        std::cout 
            << "    { Type : `" << TokenTypeToCstr(token.type) << '`'
            << ", Value : `" << token.value << '`' << " }" 
            << std::endl;
    }
}

int main(int argc, const char** argv)
{
    // Compiler options management
    std::string inputFilePath;
    {
        InputParser inputParser(argc, argv);

        if(!inputParser.CmdOptionExists("--input") && !inputParser.CmdOptionExists("-i"))
        {
            std::cerr << "ERROR : No input file provided !" << std::endl;
            return EXIT_FAILURE;
        }

        inputFilePath = inputParser.GetCmdOptions("-i", "--input");

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
        }

        if(inputParser.CmdOptionExists("--debug"))
            CompilerOptions::DebugMode = true;
        if(inputParser.CmdOptionExists("--verbose"))
            CompilerOptions::Verbose = true;
    }

    // Source file loading
    std::stringstream fileContent;
    {
        std::fstream file(inputFilePath, std::ios::in);
        fileContent << file.rdbuf();
    }

    // Some debug info
    if(CompilerOptions::Verbose)
        StdOutInputFileInfo(inputFilePath, fileContent);

    // Actual Compilation
    auto fileStr = fileContent.str();
    Tokenizer tokenizer(fileStr);
    std::vector<Token> tokens = tokenizer.Tokenize();
    
    // Some debug info
    if(CompilerOptions::DebugMode && CompilerOptions::Verbose)
        StdOutTokens(tokens);

    return EXIT_SUCCESS;
}
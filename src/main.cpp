#include <iostream>
#include <fstream>
#include <sstream>

#include "InputsParser.hpp"
#include "Tokenizer.hpp"
#include "Parser.hpp"

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 0
#define VERSION_TYPE "alpha"

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

static const char* ShortCLIUsageStr = "Usage :";
static const char* CompleteCLIUsageStr = R"(Usage :
    - `-i` or `--input`: Specifies the input log file to be compiled.
    - `-o` or `--output`: Specifies the output file path (optional, default=./a.{extention}).
    - `--format`: Specifies the output format as `JSON`, `Markdown` or `C + RayGui` format : `[json|(markdown|md)|c]` (optional, default=json).
    - `--verbose`: Enables verbose mode for detailed output (optional).
    - `--debug`: Enables debug mode for debugging information (optional).
    - `--help`: display this message, then exit.
    - `--version`: display compiler version, then exit.
)";

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
            << "    { "
            << "Type : `" << TokenTypeToCstr(token.type) << '`'
            << ", Value : `" << token.value << '`' 
            << ", pos: " << token.ln << ':' << token.col
            << " }" 
            << std::endl;
    }
}

int main(int argc, const char** argv)
{
    // Compiler options management
    std::string inputFilePath;
    {
        InputParser inputParser(argc, argv);

        if(inputParser.CmdOptionExists("--help") || inputParser.CmdOptionExists("-h"))
        {
            std::cout << CompleteCLIUsageStr << std::endl;
            return EXIT_SUCCESS;
        }

        if(inputParser.CmdOptionExists("--version") || inputParser.CmdOptionExists("-v"))
        {
            std::cout << "LogCompiler " << 'v' << VERSION_MAJOR << '.' << VERSION_MINOR << '.' << VERSION_PATCH << '-' << VERSION_TYPE << std::endl;
            return EXIT_SUCCESS;
        }

        if(!inputParser.CmdOptionExists("--input") && !inputParser.CmdOptionExists("-i"))
        {
            std::cerr << "ERROR : No input file provided !\n" << ShortCLIUsageStr << std::endl;
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
            else
            {
                std::cerr 
                    << "ERROR : invalid --format `" << format << "` is not a valid format. "
                    << "Accepted formats are [json|(markdown|md)|c]\n"
                    << ShortCLIUsageStr << std::endl;
                return EXIT_FAILURE;
            }
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

    Parser parser(tokens);
    auto program = parser.ParseProgram();
    
    return EXIT_SUCCESS;
}
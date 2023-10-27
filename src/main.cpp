#include "main.hpp"

#include <fstream>
#include <sstream>

#include "CLI.hpp"
#include "Tokenizer.hpp"
#include "Parser.hpp"

int main(int argc, const char** argv)
{
    InputParser inputParser(argc, argv);

    auto params = ProcessCLIArgs(inputParser);

    // Source file loading
    std::stringstream fileContent;
    {
        std::fstream file(params.inputFilePath, std::ios::in);
        fileContent << file.rdbuf();
    }

    // Some debug info
    if(CompilerOptions::Verbose)
        StdOutInputFileInfo(params.inputFilePath, fileContent);

    // Actual Compilation
    auto fileStr = fileContent.str();

    Tokenizer tokenizer(fileStr);
    auto tokens = tokenizer.Tokenize();

    // Some debug info
    if(CompilerOptions::DebugMode && CompilerOptions::Verbose)
        StdOutTokens(tokens);

    Parser parser(tokens);
    auto program = parser.ParseProgram();
    
    return EXIT_SUCCESS;
}
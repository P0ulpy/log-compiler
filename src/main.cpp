#include "main.hpp"

#include <fstream>
#include <sstream>

#include "CLI.hpp"
#include "Compilation/Tokenizer.hpp"
#include "Compilation/Parser.hpp"

int main(int argc, const char** argv)
{
    auto params = ProcessCLIArgs(argc, argv);

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

    // Some debug info
    if(CompilerOptions::DebugMode && CompilerOptions::Verbose)
        StdOutProgram(program);
    
    return EXIT_SUCCESS;
}
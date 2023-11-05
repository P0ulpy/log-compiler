#include "main.hpp"

#include <fstream>
#include <sstream>

#include "CLI.hpp"
#include "Compilation/Tokenizer.hpp"
#include "Compilation/Parser.hpp"

#include <chrono>

bool CompilerOptions::DebugMode = false;
bool CompilerOptions::Verbose = false;
CompilerOptions::OutputFormats CompilerOptions::OutputFormat = CompilerOptions::OutputFormats::JSON;

int main(int argc, const char** argv)
{
    auto start = std::chrono::system_clock::now();

    auto params = ProcessCLIArgs(argc, argv);

    std::stringstream fileContent;
    {
        std::fstream file(params.inputFilePath, std::ios::in);
        fileContent << file.rdbuf();
    }

    if(CompilerOptions::Verbose)
        StdOutInputFileInfo(params.inputFilePath, fileContent);

    auto fileStr = fileContent.str();

    Tokenizer tokenizer(fileStr);
    auto tokens = tokenizer.Tokenize();

    if(CompilerOptions::DebugMode && CompilerOptions::Verbose)
        StdOutTokens(tokens);

    Parser parser(tokens);
    auto program = parser.ParseProgram();

    if(CompilerOptions::DebugMode && CompilerOptions::Verbose)
        StdOutProgram(program);
    
    auto end = std::chrono::system_clock::now();
    auto elapsed = end - start;
    std::cout
        << "Compiled in : "
        << std::chrono::duration_cast<std::chrono::seconds>(elapsed).count()
        << "s "
        << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()
        << "ms"
    << std::endl;

    return EXIT_SUCCESS;
}
#include "main.hpp"

#include <fstream>
#include <sstream>

#include "CLI.hpp"
#include "Compilation/Tokenizer.hpp"
#include "Compilation/Parser.hpp"
#include "Compilation/Generation/JsonGenerator.hpp"

#include <chrono>

bool CompilerOptions::Debug = false;
bool CompilerOptions::Verbose = false;
CompilerOptions::OutputFormats CompilerOptions::OutputFormat = CompilerOptions::OutputFormats::JSON;

int main(int argc, const char** argv)
{
    auto start = std::chrono::system_clock::now();

    auto params = ProcessCLIArgs(argc, argv);

    std::stringstream fileContent;
    {
        std::fstream file(params.inputFilePath, std::ios::in);
        
        if(!file.is_open())
        {
            std::cerr 
                << "ERROR : Input file not found at `" << params.inputFilePath << "`\n" 
                << ShortCLIUsageStr << std::endl;

            exit(EXIT_FAILURE);
        }

        fileContent << file.rdbuf();
    }

    if(CompilerOptions::Verbose)
        StdOutInputFileInfo(params.inputFilePath, fileContent);

    auto fileStr = fileContent.str();

    if(CompilerOptions::Debug)
        std::cout << "Stating Tokenization phase" << std::endl;

    Tokenizer tokenizer(fileStr);
    auto tokens = tokenizer.Tokenize();

    if(CompilerOptions::Debug)
        std::cout << "Tokenization ended with `" << tokens.size() << "` token(s) found" << std::endl;
    if(CompilerOptions::Debug && CompilerOptions::Verbose)
        StdOutTokens(tokens);

    if(CompilerOptions::Debug)
        std::cout << "Stating Parsing phase" << std::endl;

    Parser parser(tokens);
    auto program = parser.ParseProgram();

    if(CompilerOptions::Debug)
        std::cout << "Parsing ended with no error" << std::endl;
    if(CompilerOptions::Debug && CompilerOptions::Verbose)
        StdOutProgram(program);

    if(CompilerOptions::Debug)
        std::cout << "Stating Generation phase for `" << OutputFormatsToCstr(CompilerOptions::OutputFormat) << '`' << std::endl;

    JsonGenerator generator(program);

    const auto& outputFile = generator.Generate();
    {
        if(params.outputFilePath.empty())
        {
            params.outputFilePath = 'a';
            params.outputFilePath += generator.GetFileExtention();
        }

        std::fstream file(params.outputFilePath, std::ios::out);
        file << outputFile.str();
    }

    if(CompilerOptions::Debug || CompilerOptions::Verbose)
        std::cout << "Sucessfully genereted output at `" << params.outputFilePath << '`' << std::endl;
    
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
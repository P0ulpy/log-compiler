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
    auto startTotal = std::chrono::system_clock::now();

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
        std::cout << "Starting Tokenization phase" << std::endl;
    auto startTokenization = std::chrono::system_clock::now();

    Tokenizer tokenizer(fileStr);
    auto tokens = tokenizer.Tokenize();

    if(CompilerOptions::Debug)
    {
        auto elapsedTokenization = std::chrono::system_clock::now() - startTokenization;

        std::cout 
            << "Tokenization ended with `" << tokens.size() << "` token(s) found in "
            << std::chrono::duration_cast<std::chrono::seconds>(elapsedTokenization).count() << "s "
            << std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTokenization).count() << "ms"
        << std::endl;

        if(CompilerOptions::Verbose)
            StdOutTokens(tokens);
    }

    if(CompilerOptions::Debug)
        std::cout << "Starting Parsing phase" << std::endl;
    auto startParsing = std::chrono::system_clock::now();

    Parser parser(tokens);
    auto program = parser.ParseProgram();
    
    if(CompilerOptions::Debug)
    {
        auto elapsedParsing = std::chrono::system_clock::now() - startParsing;

        std::cout 
            << "Parsing ended with no error in "
            << std::chrono::duration_cast<std::chrono::seconds>(elapsedParsing).count() << "s "
            << std::chrono::duration_cast<std::chrono::milliseconds>(elapsedParsing).count() << "ms"
        << std::endl;

        if(CompilerOptions::Verbose)
            StdOutProgram(program);
    }

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

    std::cout << "Genereted output at `" << params.outputFilePath << '`' << std::endl;
    
    auto elapsedTotal = std::chrono::system_clock::now() - startTotal;
    std::cout
        << "Compiled in : "
        << std::chrono::duration_cast<std::chrono::seconds>(elapsedTotal).count()<< "s "
        << std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTotal).count()<< "ms"
    << std::endl;

    return EXIT_SUCCESS;
}
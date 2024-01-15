#include "main.hpp"

#include <fstream>
#include <sstream>

#include "CLI.hpp"
#include "Compilation/Tokenizer.hpp"
#include "Compilation/Parser.hpp"
#include "Compilation/Generation/JsonGenerator.hpp"
#include "Compilation/Generation/MdGenerator.hpp"

#include <chrono>

bool CompilerOptions::Debug = false;
bool CompilerOptions::Verbose = false;
CompilerOptions::OutputFormats CompilerOptions::OutputFormat = CompilerOptions::OutputFormats::JSON;

template <class TGenerator>
requires(std::is_base_of_v<Generator, TGenerator>)
void Generate(ProgramRoot& programRoot, std::string& outputFilePath);

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
        auto elapsedTokenizationSec = std::chrono::duration_cast<std::chrono::seconds>(elapsedTokenization);
        auto elapsedTokenizationMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTokenization) - elapsedTokenizationSec;

        std::cout 
            << "Tokenization ended with `" << tokens.size() << "` token(s) found in "
            << elapsedTokenizationSec.count() << "s "
            << elapsedTokenizationMs.count() << "ms"
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
        auto elapsedParsingSec = std::chrono::duration_cast<std::chrono::seconds>(elapsedParsing);
        auto elapsedParsingMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedParsing) - elapsedParsingSec;

        std::cout 
            << "Parsing ended with no error in "
            << elapsedParsingSec.count() << "s "
            << elapsedParsingMs.count() << "ms"
        << std::endl;

        if(CompilerOptions::Verbose)
            StdOutProgram(program);
    }

    if(CompilerOptions::Debug)
        std::cout << "Stating Generation phase for `" << OutputFormatsToCstr(CompilerOptions::OutputFormat) << '`' << std::endl;
    
    switch(CompilerOptions::OutputFormat)
    {
        case CompilerOptions::OutputFormats::Markdown :
        {
            Generate<MdGenerator>(program, params.outputFilePath);
            break;
        }
        case CompilerOptions::OutputFormats::C :
        {
            std::cerr << "ERROR : C compiler output is no implemented for now !" << std::endl;
            exit(1);
            break;
        }
        case CompilerOptions::OutputFormats::JSON :
        default:
            Generate<JsonGenerator>(program, params.outputFilePath);
    }

    std::cout << "Genereted output at `" << params.outputFilePath << '`' << std::endl;

    auto elapsedTotal = std::chrono::system_clock::now() - startTotal;
    auto elapsedSec = std::chrono::duration_cast<std::chrono::seconds>(elapsedTotal);
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTotal) - elapsedSec;
    std::cout
        << "Compiled in : "
        << elapsedSec.count()<< "s "
        << elapsedMs.count()<< "ms"
    << std::endl;

    return EXIT_SUCCESS;
}

template <class TGenerator>
requires(std::is_base_of_v<Generator, TGenerator>)
void Generate(ProgramRoot& programRoot, std::string& outputFilePath)
{
    TGenerator generator(programRoot);

    const auto& outputFile = generator.Generate();
    {
        if(outputFilePath.empty())
        {
            outputFilePath = 'a';
            outputFilePath += generator.GetFileExtention();
        }

        std::fstream file(outputFilePath, std::ios::out);
        file << outputFile.str();
    }
}
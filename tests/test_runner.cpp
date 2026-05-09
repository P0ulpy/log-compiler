#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <array>
#include <string>
#include <cstring>
#include <algorithm>
#include <map>
#include <functional>

#include "../src/Compilation/Token.hpp"
#include "../src/Compilation/Tokenizer.hpp"
#include "../src/Compilation/Parser.hpp"
#include "../src/Compilation/ProgramTokens.hpp"
#include "../src/Compilation/Generation/JsonGenerator.hpp"
#include "../src/Compilation/Generation/MdGenerator.hpp"
#include "../src/Utils/InputsParser.hpp"
#include "../src/CLI.hpp"
#include "../src/main.hpp"

namespace fs = std::filesystem;

// ANSI color codes
constexpr const char* RESET  = "\033[0m";
constexpr const char* GREEN  = "\033[32m";
constexpr const char* RED    = "\033[31m";
constexpr const char* BLUE   = "\033[94m";
constexpr const char* GREY   = "\033[90m";
constexpr const char* YELLOW = "\033[33m";
constexpr const char* BOLD   = "\033[1m";

enum class TestStatus { OK, Mismatch, MissingExpected, Error };

struct TestResult 
{
    std::string caseName;
    std::string format;
    TestStatus status;
    std::string details;
};

struct FormatStats
{
    int passed = 0;
    int failed = 0;
    int missing = 0;
    int errors = 0;
    int total() const { return passed + failed + missing + errors; }
};

bool CompilerOptions::Debug = false;
bool CompilerOptions::Verbose = false;
CompilerOptions::OutputFormats CompilerOptions::OutputFormat = CompilerOptions::OutputFormats::JSON;

std::string ReadFile(const fs::path& path)
{
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

bool WriteFile(const fs::path& path, const std::string& content)
{
    fs::create_directories(path.parent_path());
    std::ofstream file(path);
    if (!file.is_open()) return false;
    file << content;
    return true;
}

std::string RunCompiler(const std::string& input, const std::string& format)
{
    Tokenizer tokenizer(input);
    auto tokens = tokenizer.Tokenize();

    Parser parser(tokens);
    auto program = parser.ParseProgram();

    if (format == "json")
    {
        JsonGenerator gen(program);
        return gen.Generate().str();
    }
    else if (format == "md")
    {
        MdGenerator gen(program);
        return gen.Generate().str();
    }

    return "";
}

std::vector<fs::path> DiscoverTestCases(const fs::path& casesDir)
{
    std::vector<fs::path> cases;

    if (!fs::exists(casesDir)) return cases;

    for (const auto& entry : fs::directory_iterator(casesDir))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".log")
        {
            cases.push_back(entry.path());
        }
    }

    std::sort(cases.begin(), cases.end());
    return cases;
}

const char* StatusLetter(TestStatus status)
{
    switch (status)
    {
        case TestStatus::OK:              return "K";
        case TestStatus::Mismatch:        return "F";
        case TestStatus::MissingExpected: return "?";
        case TestStatus::Error:           return "E";
    }
    return " ";
}

const char* StatusColor(TestStatus status)
{
    switch (status)
    {
        case TestStatus::OK:              return GREEN;
        case TestStatus::Mismatch:        return RED;
        case TestStatus::MissingExpected: return BLUE;
        case TestStatus::Error:           return RED;
    }
    return RESET;
}

void PrintStatusChar(TestStatus status)
{
    std::cout << StatusColor(status) << StatusLetter(status) << RESET;
}

void PrintLegend(size_t indent)
{
    std::string pad(indent, ' ');
    std::cout << pad << GREEN << "K" << RESET << " - passed\n";
    std::cout << pad << RED   << "F" << RESET << " - stdout mismatch\n";
    std::cout << pad << BLUE  << "?" << RESET << " - expected not recorded\n";
    std::cout << pad << RED   << "E" << RESET << " - error\n";
}

void PrintTopLabels(const std::vector<std::string>& formats, const std::map<std::string, FormatStats>& stats, size_t rowWidth)
{
    for (size_t j = 0; j < formats.size(); ++j)
    {
        std::cout << std::string(rowWidth + 2, ' ');
        for (size_t k = 0; k < j; ++k) std::cout << "| ";
        
        const auto& fmt = formats[j];
        const auto& s = stats.at(fmt);
        std::cout << "+-" << fmt;
        std::cout << " " << GREEN << "K" << RESET << ":" << s.passed;
        std::cout << " " << RED << "F" << RESET << ":" << s.failed;
        if (s.missing > 0) std::cout << " " << BLUE << "?" << RESET << ":" << s.missing;
        if (s.errors > 0) std::cout << " " << RED << "E" << RESET << ":" << s.errors;
        std::cout << "\n";
    }
}

void PrintBottomLabels(const std::vector<std::string>& formats, const std::map<std::string, FormatStats>& stats, size_t rowWidth)
{
    for (int j = static_cast<int>(formats.size()) - 1; j >= 0; --j)
    {
        std::cout << std::string(rowWidth + 2, ' ');
        for (int k = 0; k < j; ++k) std::cout << "| ";
        
        const auto& fmt = formats[j];
        const auto& s = stats.at(fmt);
        std::cout << "+-" << fmt;
        std::cout << " " << GREEN << "K" << RESET << ":" << s.passed;
        std::cout << " " << RED << "F" << RESET << ":" << s.failed;
        if (s.missing > 0) std::cout << " " << BLUE << "?" << RESET << ":" << s.missing;
        if (s.errors > 0) std::cout << " " << RED << "E" << RESET << ":" << s.errors;
        std::cout << "\n";
    }
}

constexpr const char* TestShortUsageStr = "Usage : log-compiler-tests <replay|record> [--dir path] [--format json|md|all] [--verbose] [--debug-output] [---feature-coverage] [-v] [-h]";

constexpr const char* TestCompleteUsageStr = 
R"(positional arguments:
  replay            Run tests and compare against expected outputs
  record            Run tests and record outputs as expected

optional arguments:
  --dir <path>         Test cases directory (default: tests/cases)
  --format <fmt>       Only test specific format: json, md, all (default: all)
  --verbose            Show detailed diff on mismatch
  --debug-output       Exercise compiler debug/verbose output paths
  --feature-coverage   Show feature/phase coverage analysis
  -v, --version        Display test runner version, then exit.
  -h, --help           Display this message, then exit.
)";

struct TestCLIParameters
{
    std::string action = "replay";
    std::string casesDir = "tests/cases";
    std::string expectedDir = "tests/expected";
    std::string formatFilter = "all";
    bool verbose = false;
    bool featureCoverage = false;
    bool debugOutput = false;
};

TestCLIParameters ProcessTestCLIArgs(int argc, const char** argv)
{
    InputParser inputParser(argc, argv);

    if(inputParser.CmdOptionExists("--help") || inputParser.CmdOptionExists("-h"))
    {
        std::cout 
            << TestShortUsageStr << '\n' 
            << TestCompleteUsageStr 
            << std::endl;
        exit(EXIT_SUCCESS);
    }

    if(inputParser.CmdOptionExists("--version") || inputParser.CmdOptionExists("-v"))
    {
        std::cout << "LogCompiler Tests v" << VERSION_MAJOR << '.' << VERSION_MINOR << '.' << VERSION_PATCH << '-' << VERSION_TYPE << std::endl;
        exit(EXIT_SUCCESS);
    }

    TestCLIParameters params;

    // Positional action
    if(inputParser.CmdIndexExists(0))
    {
        std::string action = inputParser[static_cast<size_t>(0)];
        if(action == "replay" || action == "record")
            params.action = action;
    }

    if(inputParser.CmdOptionExists("--dir"))
        params.casesDir = inputParser.GetCmdOption("--dir");

    if(inputParser.CmdOptionExists("--format"))
        params.formatFilter = inputParser.GetCmdOption("--format");

    if(inputParser.CmdOptionExists("--verbose"))
        params.verbose = true;

    if(inputParser.CmdOptionExists("--debug-output"))
        params.debugOutput = true;

    if(inputParser.CmdOptionExists("--feature-coverage"))
        params.featureCoverage = true;

    return params;
}

std::string EscapeForDisplay(const std::string& str, size_t maxLen = 500)
{
    std::string out;
    for (size_t i = 0; i < str.size() && i < maxLen; ++i)
    {
        switch (str[i])
        {
            case '\n': out += "\\n\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: out += str[i];
        }
    }
    if (str.size() > maxLen) out += "...";
    return out;
}

// --- Feature Coverage Analysis ---
struct FeatureCoverage
{
    std::string feature;
    int coveredBy = 0;      // number of test cases exercising this
    std::vector<std::string> testCases;
};

void AnalyzeCoverage(const std::vector<fs::path>& cases, size_t maxNameLen)
{
    // Define features to track
    struct FeatureDetector
    {
        std::string name;
        std::function<bool(const std::string&, const std::vector<Token>&, const ProgramRoot&)> detect;
    };

    std::array<FeatureDetector, 16> features = {{
        { "Empty input",         [](const std::string& input, const std::vector<Token>&, const ProgramRoot&) { return input.empty() || input.find_first_not_of(" \n\r\t") == std::string::npos; }},
        { "TextLine tokens",     [](const std::string&, const std::vector<Token>& tokens, const ProgramRoot&) { for(auto& t : tokens) if(t.type == TokenType::TextLine) return true; return false; }},
        { "TitleSymbol tokens",  [](const std::string&, const std::vector<Token>& tokens, const ProgramRoot&) { for(auto& t : tokens) if(t.type == TokenType::TitleSymbol) return true; return false; }},
        { "QuoteBlock tokens",   [](const std::string&, const std::vector<Token>& tokens, const ProgramRoot&) { for(auto& t : tokens) if(t.type == TokenType::QuoteBlockLine) return true; return false; }},
        { "EmptyLine tokens",    [](const std::string&, const std::vector<Token>& tokens, const ProgramRoot&) { for(auto& t : tokens) if(t.type == TokenType::EmptyLine) return true; return false; }},
        { "Escape at line start",[](const std::string& input, const std::vector<Token>&, const ProgramRoot&) { return input.find("\n\\#") != std::string::npos || input.find("\n\\>") != std::string::npos || input.substr(0, 2) == "\\#" || input.substr(0, 2) == "\\>"; }},
        { "Line continuation",   [](const std::string& input, const std::vector<Token>&, const ProgramRoot&) {
            // Find \ at end of a line followed by more text
            size_t pos = 0;
            while ((pos = input.find('\\', pos)) != std::string::npos) {
                if (pos + 1 < input.size() && (input[pos+1] == '\n' || input[pos+1] == '\r'))
                    return true;
                ++pos;
            }
            return false;
        }},
        { "Inline links",        [](const std::string& input, const std::vector<Token>&, const ProgramRoot&) {
            // Look for [...](...) pattern
            size_t pos = 0;
            while ((pos = input.find('[', pos)) != std::string::npos) {
                size_t close = input.find(']', pos);
                if (close != std::string::npos && close + 1 < input.size() && input[close+1] == '(') {
                    size_t pclose = input.find(')', close);
                    if (pclose != std::string::npos) return true;
                }
                ++pos;
            }
            return false;
        }},
        { "Escaped brackets",    [](const std::string& input, const std::vector<Token>&, const ProgramRoot&) { return input.find("\\[") != std::string::npos || input.find("\\]") != std::string::npos; }},
        { "Multiple H1 sections",[](const std::string&, const std::vector<Token>& tokens, const ProgramRoot&) {
            int h1count = 0;
            for (size_t i = 0; i < tokens.size(); ++i) {
                if (tokens[i].type == TokenType::TitleSymbol && tokens[i].value == "#") ++h1count;
            }
            return h1count > 1;
        }},
        { "Deep nesting (4+)",   [](const std::string&, const std::vector<Token>& tokens, const ProgramRoot&) {
            for (auto& t : tokens) {
                if (t.type == TokenType::TitleSymbol && t.value.size() >= 4) return true;
            }
            return false;
        }},
        { "Text before heading", [](const std::string&, const std::vector<Token>& tokens, const ProgramRoot&) {
            for (auto& t : tokens) {
                if (t.type == TokenType::TitleSymbol) return false;
                if (t.type == TokenType::TextLine) return true;
            }
            return false;
        }},
        { "Consecutive quotes",  [](const std::string&, const std::vector<Token>& tokens, const ProgramRoot&) {
            int consecutive = 0;
            for (auto& t : tokens) {
                if (t.type == TokenType::QuoteBlockLine) { ++consecutive; if (consecutive >= 2) return true; }
                else consecutive = 0;
            }
            return false;
        }},
        { "Special JSON chars",  [](const std::string& input, const std::vector<Token>&, const ProgramRoot&) {
            return input.find('"') != std::string::npos || input.find('\\') != std::string::npos || input.find('<') != std::string::npos || input.find('&') != std::string::npos;
        }},
        { "Long lines (100+)",   [](const std::string& input, const std::vector<Token>&, const ProgramRoot&) {
            std::istringstream iss(input);
            std::string line;
            while (std::getline(iss, line)) {
                if (line.size() > 100) return true;
            }
            return false;
        }},
        { "Multiple paragraphs", [](const std::string&, const std::vector<Token>& tokens, const ProgramRoot&) {
            int textBlocks = 0;
            bool inText = false;
            for (auto& t : tokens) 
            {
                if (t.type == TokenType::TextLine) 
                {
                    if (!inText) 
                    { 
                        ++textBlocks; inText = true; 
                    }
                } 
                else 
                {
                    inText = false;
                }
            }
            return textBlocks >= 2;
        }},
    }};

    std::array<FeatureCoverage, features.size()> coverageResults;
    for (size_t i = 0; i < features.size(); ++i)
    {
        coverageResults[i] = {
            .feature = features[i].name, 
            .coveredBy = 0, 
            .testCases = {} 
        };
    }

    for (const auto& casePath : cases)
    {
        std::string caseName = casePath.stem().string();
        std::string input = ReadFile(casePath);

        Tokenizer tokenizer(input);
        auto tokens = tokenizer.Tokenize();

        Parser parser(tokens);
        auto program = parser.ParseProgram();

        for (size_t fi = 0; fi < features.size(); ++fi)
        {
            if (features[fi].detect(input, tokens, program))
            {
                coverageResults[fi].coveredBy++;
                coverageResults[fi].testCases.push_back(caseName);
            }
        }
    }

    // Print feature coverage report
    std::cout << "\n";
    std::cout << BOLD << "  Feature Coverage Analysis" << RESET << "\n\n";

    size_t maxFeatureLen = 0;
    for (auto& c : coverageResults)
        maxFeatureLen = std::max(maxFeatureLen, c.feature.size());

    int covered = 0, total = static_cast<int>(coverageResults.size());

    for (auto& c : coverageResults)
    {
        const char* color = (c.coveredBy > 0) ? GREEN : RED;
        std::string bar;
        int barLen = std::min(c.coveredBy, 20);
        for (int i = 0; i < barLen; ++i) bar += "=";

        std::cout << "  " << color
                  << std::string(maxFeatureLen - c.feature.size(), ' ') << c.feature
                  << RESET << ": "
                  << color << bar << RESET
                  << " (" << c.coveredBy << " test" << (c.coveredBy != 1 ? "s" : "") << ")\n";

        if (c.coveredBy > 0) ++covered;
    }

    std::cout << "\n";
    float pct = (total > 0) ? (100.0f * covered / total) : 0.0f;
    const char* summaryColor = (pct >= 90.0f) ? GREEN : (pct >= 70.0f) ? YELLOW : RED;
    std::cout << "  " << summaryColor << "Feature coverage: " << covered << "/" << total
              << " (" << static_cast<int>(pct) << "%)" << RESET << "\n\n";
}

int main(int argc, const char** argv)
{
    auto params = ProcessTestCLIArgs(argc, argv);

    std::vector<std::string> formats;
    if (params.formatFilter == "all") { formats = { "json", "md" }; }
    else { formats = { params.formatFilter }; }

    auto cases = DiscoverTestCases(params.casesDir);

    if (cases.empty())
    {
        std::cerr << "No test cases found in " << params.casesDir << "\n";
        return 1;
    }

    std::vector<TestResult> results;
    std::map<std::string, FormatStats> statsByFormat;
    for (const auto& fmt : formats) statsByFormat[fmt] = {};

    int totalPassed = 0, totalFailed = 0, totalRecorded = 0, totalErrors = 0;

    // Compute column width
    size_t maxNameLen = 0;
    for (const auto& c : cases)
    {
        maxNameLen = std::max(maxNameLen, c.stem().string().size());
    }

    // --- Report Header ---
    std::cout << "\n";
    std::cout << BOLD << "  Log Compiler Test Suite" << RESET << "\n";
    std::cout << "  Action: " << YELLOW << params.action << RESET << "\n\n";
    PrintLegend(maxNameLen + 2);
    std::cout << "\n";

    // Top labels with stats placeholders (we'll print after)
    // First pass: run all tests
    struct RowResult
    {
        std::string caseName;
        std::vector<TestStatus> statuses;
    };
    std::vector<RowResult> rowResults;

    for (const auto& casePath : cases)
    {
        std::string caseName = casePath.stem().string();
        std::string input = ReadFile(casePath);

        RowResult row { caseName, {} };

        for (const auto& format : formats)
        {
            fs::path expectedPath = fs::path(params.expectedDir) / (caseName + "." + format);

            std::string actual;
            try
            {
                actual = RunCompiler(input, format);
            }
            catch (const std::exception& e)
            {
                results.push_back({ caseName, format, TestStatus::Error, e.what() });
                row.statuses.push_back(TestStatus::Error);
                statsByFormat[format].errors++;
                ++totalErrors;
                continue;
            }

            if (params.action == "record")
            {
                WriteFile(expectedPath, actual);
                results.push_back({ caseName, format, TestStatus::OK, "" });
                row.statuses.push_back(TestStatus::OK);
                statsByFormat[format].passed++;
                ++totalRecorded;
            }
            else // replay
            {
                if (!fs::exists(expectedPath))
                {
                    results.push_back({ caseName, format, TestStatus::MissingExpected, "" });
                    row.statuses.push_back(TestStatus::MissingExpected);
                    statsByFormat[format].missing++;
                    ++totalFailed;
                }
                else
                {
                    std::string expected = ReadFile(expectedPath);
                    if (actual == expected)
                    {
                        results.push_back({ caseName, format, TestStatus::OK, "" });
                        row.statuses.push_back(TestStatus::OK);
                        statsByFormat[format].passed++;
                        ++totalPassed;
                    }
                    else
                    {
                        std::string detail;
                        if (params.verbose)
                        {
                            detail = "EXPECTED:\n" + EscapeForDisplay(expected) + "\nACTUAL:\n" + EscapeForDisplay(actual);
                        }
                        results.push_back({ caseName, format, TestStatus::Mismatch, detail });
                        row.statuses.push_back(TestStatus::Mismatch);
                        statsByFormat[format].failed++;
                        ++totalFailed;
                    }
                }
            }
        }

        rowResults.push_back(row);
    }

    // --- Print Report Table ---
    PrintTopLabels(formats, statsByFormat, maxNameLen);
    std::cout << "\n";

    for (const auto& row : rowResults)
    {
        std::cout << std::string(maxNameLen - row.caseName.size(), ' ') << row.caseName << ": ";
        for (const auto& status : row.statuses)
        {
            PrintStatusChar(status);
            std::cout << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\n";
    PrintBottomLabels(formats, statsByFormat, maxNameLen);

    // --- Summary ---
    std::cout << "\n";
    size_t totalTests = totalPassed + totalFailed + totalErrors + totalRecorded;

    if (params.action == "record")
    {
        std::cout << GREEN << "  Recorded " << totalRecorded << " expected output(s)" << RESET << "\n";
    }
    else
    {
        std::cout << "  Total: " << totalTests << " | "
                  << GREEN << "Passed: " << totalPassed << RESET << " | "
                  << RED << "Failed: " << totalFailed << RESET << " | "
                  << RED << "Errors: " << totalErrors << RESET << "\n";

        if (totalFailed == 0 && totalErrors == 0)
        {
            std::cout << "\n  " << GREEN << "All tests passed!" << RESET << "\n";
        }
    }
    std::cout << "\n";

    // Print failure details
    if (params.verbose)
    {
        for (const auto& r : results)
        {
            if (r.status == TestStatus::Mismatch && !r.details.empty())
            {
                std::cout << RED << "  FAIL" << RESET << " " << r.caseName << " [" << r.format << "]:\n";
                std::cout << r.details << "\n\n";
            }
            else if (r.status == TestStatus::Error)
            {
                std::cout << RED << "  ERROR" << RESET << " " << r.caseName << " [" << r.format << "]: " << r.details << "\n\n";
            }
        }
    }

    // Feature coverage analysis
    if (params.featureCoverage)
    {
        AnalyzeCoverage(cases, maxNameLen);
    }

    // Debug output test: enables compiler Debug+Verbose flags and exercises
    // operator<< for Token, ProgramTokens, InlineContent, StdOutTokens, StdOutProgram
    if (params.debugOutput)
    {
        // Enable compiler debug flags to exercise Parser debug path
        CompilerOptions::Debug = true;
        CompilerOptions::Verbose = true;

        // Redirect cout/cerr to capture debug output without polluting test results
        std::ostringstream captured;
        std::streambuf* coutBuf = std::cout.rdbuf(captured.rdbuf());
        std::streambuf* cerrBuf = std::cerr.rdbuf(captured.rdbuf());

        for (const auto& casePath : cases)
        {
            std::string input = ReadFile(casePath);
            Tokenizer tokenizer(input);
            auto tokens = tokenizer.Tokenize();

            // Exercise StdOutTokens (calls Token operator<< for each token)
            StdOutTokens(tokens);

            // Parser with Debug=true exercises the debug warn path
            Parser parser(tokens);
            auto program = parser.ParseProgram();

            // Exercise StdOutProgram (calls operator<< for all ProgramToken types)
            StdOutProgram(program);
        }

        // Restore cout/cerr
        std::cout.rdbuf(coutBuf);
        std::cerr.rdbuf(cerrBuf);

        // Reset flags
        CompilerOptions::Debug = false;
        CompilerOptions::Verbose = false;

        std::cout << GREEN << "  Debug output paths exercised successfully (" << cases.size() << " cases, "
                  << captured.str().size() << " bytes of debug output)" << RESET << "\n\n";
    }

    return (totalFailed > 0 || totalErrors > 0) ? 1 : 0;
}

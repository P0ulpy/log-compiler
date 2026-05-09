#include "Parser.hpp"

#include "../main.hpp"

#include <iostream>
#include <cassert>
#include <limits>
#include <cstdint>

Parser::Parser(std::vector<Token>& tokens)
    : m_tokens(tokens)
{}

ProgramRoot Parser::ParseProgram(const std::string_view& name)
{
    ProgramRoot rootNode {
        .name = std::string(name),
    };

    while(CanPeek())
    {
        if(auto programToken = ParseNext())
        {
            rootNode.content.push_back(programToken.value());
        }
    }

    return rootNode;
}

const std::optional<ProgramTokenVariant> Parser::ParseNext()
{
    if(!CanPeek())
        return {};

    ParserTokenVisitor visitor(*this);
    Token current = Peek();

    switch(current.type)
    {
        case TokenType::TextLine:       return visitor.operator()<TokenType::TextLine>();
        case TokenType::TitleSymbol:    return visitor.operator()<TokenType::TitleSymbol>();
        case TokenType::QuoteBlockLine: return visitor.operator()<TokenType::QuoteBlockLine>();
        case TokenType::EmptyLine:      return visitor.operator()<TokenType::EmptyLine>();
        case TokenType::TextLiteral:    return visitor.operator()<TokenType::TextLiteral>();
        default: _STL_UNREACHABLE;
    }

    if(CompilerOptions::Debug)
        std::cerr << "Parsing WARN : Unhandled Token, `" << Peek() << '`' << '\n';
    
    Consume();
    return {};
}

// --- ParserTokenVisitor specializations ---

ParserTokenVisitor::ParserTokenVisitor(Parser& parser)
    : m_parser(parser)
{}

template <>
std::optional<ProgramTokenVariant> ParserTokenVisitor::operator()<TokenType::TextLine>()
{
    auto textLine = m_parser.TryConsume(TokenType::TextLine);
    if(!textLine) return {};

    TextBlockToken textBlockToken {};

    std::string currentLineText = textLine.value().value;

    while(const auto& subTextLine = m_parser.TryConsume(TokenType::TextLine))
    {           
        if(currentLineText.ends_with('\\'))
        {
            // Backslash at end of line = line break
            currentLineText.pop_back();
            textBlockToken.lines.push_back(ParseInlineContent(currentLineText));
            currentLineText = subTextLine.value().value;
        }
        else
        {
            currentLineText.append(' ' + subTextLine.value().value);
        }
    }

    textBlockToken.lines.push_back(ParseInlineContent(currentLineText));
    return textBlockToken;
}

template <>
std::optional<ProgramTokenVariant> ParserTokenVisitor::operator()<TokenType::TitleSymbol>()
{
    auto titleSymbolOpt = m_parser.TryConsume(TokenType::TitleSymbol);
    if(!titleSymbolOpt) return {};

    uint16_t titleLevel = m_parser.ComputeTitleLevel(titleSymbolOpt.value());

    TitleToken titleToken {
        .level = titleLevel
    };

    auto textLiteral = m_parser.MustConsume(TokenType::TextLiteral);
    titleToken.text = textLiteral.value().value;

    // Content tokens
    NodeToken nodeToken {
        .title = titleToken
    };

    while(m_parser.CanPeek())
    {
        if(auto titleSymbolOpt = m_parser.TryPeek(TokenType::TitleSymbol))
        {
            uint16_t innerTitleLevel = m_parser.ComputeTitleLevel(titleSymbolOpt.value());

            if(innerTitleLevel <= titleLevel)
                return nodeToken;
        }

        if(auto programTokenOtp = m_parser.ParseNext())
        {
            nodeToken.content.push_back(programTokenOtp.value());
        }
    }

    return nodeToken;
}

template <>
std::optional<ProgramTokenVariant> ParserTokenVisitor::operator()<TokenType::QuoteBlockLine>()
{
    auto quoteLine = m_parser.TryConsume(TokenType::QuoteBlockLine);
    if(!quoteLine) return {};

    QuoteBlockToken quoteBlockToken {};

    quoteBlockToken.lines.push_back(ParseInlineContent(quoteLine.value().value));

    while(const auto& subQuoteLine = m_parser.TryConsume(TokenType::QuoteBlockLine))
    {
        quoteBlockToken.lines.push_back(ParseInlineContent(subQuoteLine.value().value));
    }

    return quoteBlockToken;
}

template <>
std::optional<ProgramTokenVariant> ParserTokenVisitor::operator()<TokenType::EmptyLine>()
{
    (void)m_parser.TryConsume(TokenType::EmptyLine);
    return {};
}

template <>
std::optional<ProgramTokenVariant> ParserTokenVisitor::operator()<TokenType::TextLiteral>()
{
    // TextLiteral appearing outside of a title context — skip it
    (void)m_parser.Consume();
    return {};
}

// --- Parser utility methods ---

uint16_t Parser::ComputeTitleLevel(const Token& titleSymbolToken)
{
    assert(titleSymbolToken.value.size() < std::numeric_limits<uint16_t>::max());
    
    uint16_t titleLevel = static_cast<uint16_t>(titleSymbolToken.value.size());

    if(titleLevel == 0 || titleLevel > 6)
    {
        std::cerr 
            << "Parsing ERROR : Invalid title level : `" << titleSymbolToken 
            << "` for token " << TokenError(titleSymbolToken) 
            << '\n';
            
        exit(EXIT_FAILURE);
    }

    return titleLevel;
}

bool Parser::TryPeek(Token &outToken, int16_t offset) const
{
    if(CanPeek(offset))
    {
        outToken = Peek(offset);
        return true;
    }

    return false;
}

const std::optional<Token> Parser::TryPeek(int16_t offset) const
{
    if(CanPeek(offset))
    {
        return Peek(offset);
    }

    return {};
}

const std::optional<Token> Parser::TryPeek(TokenType type, int16_t offset) const
{
    Token token;
    if(TryPeek(token) && token.type == type)
    {
        return Peek(offset);
    }

    return {};
}

const std::optional<Token> Parser::MustConsume(TokenType type, const std::string &errorMsg)
{
    if(auto tokenOpt = TryPeek())
    {
        auto& token = tokenOpt.value();

        if(token.type != type)
        {
            std::cerr
                << errorMsg << '\n'
                << "Expected token : `" << TokenTypeToCstr(type) << '`' << '\n'
                << "Unexpexted token " << TokenError(token)
                << '\n';
        }
        else
        {
            return Consume();
        }
    }
    else
    {
        std::cerr 
            << errorMsg << '\n' 
            << "Unexpected End of file : Expected `" << TokenTypeToCstr(type) << "` token" << '\n';
        exit(EXIT_FAILURE);
    }

    return {};
}

const std::optional<Token> Parser::TryConsume(TokenType type)
{
    if(TryPeek(type))
    {
        return Consume();
    }

    return {};
}

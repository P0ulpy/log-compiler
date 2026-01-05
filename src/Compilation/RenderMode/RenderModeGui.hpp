#pragma once

#include <raylib.h>
#include <raygui.h>
#include <concepts>

#include "../ProgramTokens.hpp"

class RenderVisitor
{
public:
    RenderVisitor() = default;

    template<typename TToken>
    void operator()(const TToken& value);

private:
    float Offset = 0;
};

void RenderModeContent(RenderVisitor& visitor, const NodeContent &nodeContent);

template <> void RenderVisitor::operator()<NodeToken>(const NodeToken& value);
template <> void RenderVisitor::operator()<TextBlockToken>(const TextBlockToken& value);
template <> void RenderVisitor::operator()<QuoteBlockToken>(const QuoteBlockToken& value);

template <class TToken>
inline void RenderVisitor::operator()(const TToken &value)
{
    static_assert(assert_false_v<TToken>, "Unhandled Token Type, please add a visitor specialisation for this type");
}

#define MARGIN_X 10
#define MARGIN_Y 3
#define HEIGHT 15

template <>
void RenderVisitor::operator()(const NodeToken &token)
{
    Rectangle rect = {
        .x = MARGIN_X,
        .y = Offset + MARGIN_Y,
        .width = (float)GetScreenWidth(),
        .height = HEIGHT
    };

    GuiLabel(rect, token.title.text.c_str());
    Offset += rect.height;

    RenderModeContent(*this, token.content);
}

template <>
void RenderVisitor::operator()(const TextBlockToken &value)
{
    for(auto& line : value.lines)
    {
        Rectangle rect = {
            .x = MARGIN_X,
            .y = Offset + MARGIN_Y,
            .width = (float)GetScreenWidth(),
            .height = HEIGHT
        };

        GuiLabel(rect, line.c_str());
        Offset += rect.height;
    }
}

template <>
void RenderVisitor::operator()(const QuoteBlockToken &value)
{
    for(auto& line : value.lines)
    {
        Rectangle rect = {
            .x = MARGIN_X,
            .y = Offset + MARGIN_Y,
            .width = (float)GetScreenWidth(),
            .height = HEIGHT
        };

        GuiLabel(rect, line.c_str());
        Offset += rect.height;
    }
}

void RenderModeContent(RenderVisitor& visitor, const NodeContent &nodeContent)
{
    for(auto& node : nodeContent)
    {
        std::visit([&](auto&& token)
        {
            visitor(token);
        }, node);
    }
}

void RenderMode(ProgramRoot& programRoot)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1080, 720, "Hello Raylib !");
    SetTargetFPS(144);

    // GuiSetStyle(DEFAULT, TEXT_SIZE, 25);

    while(!WindowShouldClose())
    {
        RenderVisitor visitor;

        BeginDrawing();
            ClearBackground(WHITE);
            RenderModeContent(visitor, programRoot.content);
        EndDrawing();
    }

    CloseWindow();
}
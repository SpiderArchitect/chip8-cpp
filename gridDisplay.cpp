#include "gridDisplay.hpp"
#include "chip8.hpp"
#include <raylib.h>
#include <cstdint>
#include <vector>
#include <utility>

GridDisplay::GridDisplay(int rowCount, int colCount, int cellSize, char const* title) : rowCount(rowCount), colCount(colCount), cellSize(cellSize) {
    InitWindow(colCount * cellSize, rowCount * cellSize, title);
    SetTargetFPS(60);
}

GridDisplay::~GridDisplay()
{
    CloseWindow();
}

void GridDisplay::UpdateDisplay(void const* buffer)
{
    const uint8_t* pixels = (const uint8_t*)buffer;
    BeginDrawing();
    ClearBackground(BLACK);
    for(int r = 0; r < rowCount; ++r)
    {
        for(int c = 0; c < colCount; ++c)
        {
            if(pixels[r * colCount + c] != 0) DrawRectangle(c * cellSize, r * cellSize, cellSize, cellSize, WHITE);
        }
    }
    EndDrawing();
}

void GridDisplay::processKeyboardInput(Chip8& chip8)
{
    std::vector<std::pair<KeyboardKey, int>> keyMapping = {
        {KEY_ONE, 0x1}, {KEY_TWO, 0x2}, {KEY_THREE, 0x3}, {KEY_FOUR, 0xC},
        {KEY_Q, 0x4}, {KEY_W, 0x5}, {KEY_E, 0x6}, {KEY_R, 0xD},
        {KEY_A, 0x7}, {KEY_S, 0x8}, {KEY_D, 0x9}, {KEY_F, 0xE},
        {KEY_Z, 0xA}, {KEY_X, 0x0}, {KEY_C, 0xB}, {KEY_V, 0xF}
    };
    for(const auto &[key, mapping]: keyMapping)
    {
        if(IsKeyDown(key)) chip8.PressKey(mapping);
        else chip8.ReleaseKey(mapping);
    }
}
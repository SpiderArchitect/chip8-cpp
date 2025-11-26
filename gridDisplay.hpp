#pragma once

class Chip8;

class GridDisplay {
private:
    int rowCount;
    int colCount;
    int cellSize;
    
public:
    GridDisplay(int rowCount, int colCount, int cellSize, char const* title);
    ~GridDisplay();
    void UpdateDisplay(void const* buffer);
    void processKeyboardInput(Chip8& chip8);
};
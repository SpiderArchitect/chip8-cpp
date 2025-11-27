#include "chip8.hpp"
#include "gridDisplay.hpp"
#include "raylib.h"
#include <iostream>
int main(int argc, char** argv) {
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <ROM_FILE>" << std::endl;
        return 1; // Return non-zero to signal error to the OS
    }
    char const* rom = argv[1];
    Chip8 chip8;
    try 
    {
        chip8.LoadROM(rom);
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "CRITICAL ERROR: Failed to load ROM." << std::endl;
        std::cerr << "Reason: " << e.what() << std::endl;
        return 1;
    }
    GridDisplay display(32, 64, 10, "Chip8 Emulation");
    while(!WindowShouldClose())
    {
        // Event handling
        display.processKeyboardInput(chip8);

        // Update state
        try 
        {
            for (int i = 0; i < 10; ++i)
            {
                chip8.Tick();
            }
        } 
        catch (const std::exception& e) 
        {
            std::cerr << "CRITICAL ERROR: Emulation crashed." << std::endl;
            std::cerr << "Reason: " << e.what() << std::endl;
            return 1;
        }
        chip8.DecrementTimers();

        // Display
        display.UpdateDisplay(chip8.getVideo());
    }
}
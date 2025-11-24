#include <cstdint>
#pragma once
class Chip8 {
private:
    // memory address is from 0x000 to 0xFFF
    // address 0x000 to 0x1FF is for original interpreter code and is not used by programs
    // programs use fom 0x200
    uint8_t memory[4096];

    // in interpreter section of memory address fonts are stored, font start is address of begining of font start
    uint16_t fontStart = 0x000;

    // 16 general purpose 8 bit registers represented by Vx (x = 0, 1, ... E, F)
    // VF is not written to directly as it is used as flags by some instructions
    uint8_t V[16];

    // Index register stores some address of memory
    // 12 bits are required to store from 0x000 to 0xFFF (each hex digit can be represented in 4 bits), 16 bits are used 4 are wasted for word alignment
    uint16_t I;
    
    // Program counter stores address of currently execting instruction, 12 bits required 16 bits used
    uint16_t PC;

    // Stack for nested calls, only till 16 levels supported
    // stores address to return to after a nested function ends
    uint16_t stack[16];

    // Stack pointer
    uint8_t SP;

    // Timer registers
    // delay timer counts down till 0 for timing
    uint8_t DT;
    // sound timer, buzzes if value greater than 0
    uint8_t ST;

    // A 64 height, 32 width monochorme display which wraps
    uint32_t display[64 * 32];

    // 0,1 ... F are keys available for chip 8, 0 means not pressed, 1 means pressed
    uint16_t keypad;
public:
    Chip8();
    void LoadROM(char const* filename);
};
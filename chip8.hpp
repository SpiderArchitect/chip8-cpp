#include <cstdint>
#include <random>

#pragma once

struct Instruction {
    uint16_t opcode;
    uint16_t type; // 1st nibble, categorize opcodes, is 16 bits for easier comparisions
    uint8_t x; // 2nd nibble gives register1 address if applicable
    uint8_t y; // 3rd nibble gives register2 address if applicable
    uint8_t n; // 4th nibble
    uint8_t nn; // last 8 bits
    uint16_t nnn; // last 12 bits
};
class Chip8 {
public:
    Chip8();
    void LoadROM(char const* filename);
    void PressKey(uint8_t key);
    void ReleaseKey(uint8_t key);

    // Fetch decode execute a single instruction from memory
    void Tick();
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
    uint8_t display[64 * 32];

    // 0,1 ... F are keys available for chip 8, 0 means not pressed, 1 means pressed
    uint16_t keypad;

    // fetches 16 bit instruction from memory
    uint16_t Fetch();

    // decode it into nibbles
    Instruction Decode(uint16_t opcode);

    // execute instruction
    void Execute(Instruction instr);

    // for random var
    std::default_random_engine randGen;
    std::uniform_int_distribution<int> randByte;

    // error
    void Error(char* msg) {
        std::cerr<<"[CHIP8 ERROR] "<<msg<<", PC at "<<PC-0x200<<std::endl;
        throw std::runtime_error(msg);
    }
};
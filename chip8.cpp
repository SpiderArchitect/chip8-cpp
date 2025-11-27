#include "chip8.hpp"
#include <iostream>
#include <cstring>
#include <ctime>
#include <fstream>

// fonts are stored in interpereter area of chip8 memory
uint8_t font[(0xF+1) * 5] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


Chip8::Chip8() : I(0), SP(0), DT(0), ST(0), PC(0x200), keypad(0), randGen{time(0)}, randByte{0, 255}
{
    memcpy(&memory[fontStart], font, sizeof(font));
    memset(V, 0, sizeof(V));
    memset(video, 0, sizeof(video));
    memset(stack, 0, sizeof(stack));
}

void Chip8::LoadROM(const char * filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if(!file.is_open()) Error("Error opening file");
    int size = file.tellg();
    if(size > (0xFFF - 0x200 + 1)) Error("ROM size too large");
    file.seekg(0, std::ios::beg);
    file.read((char*)&memory[0x200], size);
}

void Chip8::PressKey(uint8_t key)
{
    if(key > 0xF) return;
    keypad |= (1 << key);
}

void Chip8::ReleaseKey(uint8_t key)
{
    if(key > 0xF) return;
    keypad &= (~(1 << key));
}

void Chip8::DecrementTimers()
{
    if(DT > 0) --DT;
    if(ST > 0) --ST;
}

void Chip8::Tick()
{
    // implement fetch decode execute cycle
    // chip8 instrctions are 16 bit long
    uint16_t opcode = Fetch();
    PC += 2;
    Instruction instr = Decode(opcode);
    Execute(instr);
}

uint16_t Chip8::Fetch()
{
    if(PC+1 > 0xFFF) Error("Memory Access Violation");
    return (memory[PC] << 8) | memory[PC+1];
}

Instruction Chip8::Decode(uint16_t opcode)
{
    Instruction instr;
    instr.opcode = opcode;
    instr.type = opcode & 0xF000;
    instr.x = (opcode >> 8) & 0x000F;
    instr.y = (opcode >> 4) & 0x000F;
    instr.n = opcode & 0x000F;
    instr.nn = opcode & 0x00FF;
    instr.nnn = opcode & 0x00FFF;
    return instr;
}

void Chip8::Execute(Instruction instr)
{
    switch(instr.type)
    {
        case 0x0000:
        {
            switch(instr.nnn)
            {
                case 0x0E0: memset(video, 0, sizeof(video)); break;
                case 0x0EE:
                {
                    if(SP == 0) Error("Stack Underflow");
                    --SP;
                    PC = stack[SP];
                    break;
                }
            }
            break;
        }
        case 0x1000:
            // jump to address nnn
            PC = instr.nnn;
            break;
        case 0x2000:
        {
            // jump to address nnn but store curent PC in stack to return to it later
            if(SP >= 16) Error("Stack Overflow");
            stack[SP] = PC;
            SP++;
            PC = instr.nnn;
            break;
        }
        case 0x3000:
            // skip next instruction if V[x] == nn
            if(V[instr.x] == instr.nn) PC += 2;
            break;
        case 0x4000:
            // skip next instruction if V[x] != nn
            if(V[instr.x] != instr.nn) PC += 2;
            break;
        case 0x5000:
            // skip next instruction if V[x] == V[y]
            if(V[instr.x] == V[instr.y]) PC += 2;
            break;
        case 0x6000: V[instr.x] = instr.nn; break;
        case 0x7000: V[instr.x] += instr.nn; break;
        case 0x8000:
        {

            switch(instr.n)
            {
                case 0x0: V[instr.x] = V[instr.y]; break;
                case 0x1: V[instr.x] |= V[instr.y]; break;
                case 0x2: V[instr.x] &= V[instr.y]; break;
                case 0x3: V[instr.x] ^= V[instr.y]; break;
                // important case here is when x = 0xF, hence flag is calculated first
                case 0x4:
                {
                    int8_t carry = V[instr.y] > (0xFF - V[instr.x]) ? 1 : 0;
                    V[instr.x] += V[instr.y];
                    V[0xF] = carry;
                    break;
                }
                case 0x5:
                {
                    int8_t notCarry = V[instr.x] >= V[instr.y] ? 1 : 0;
                    V[instr.x] -= V[instr.y];
                    V[0xF] = notCarry;
                    break;
                }
                case 0x6:
                {
                    int8_t lsb = V[instr.x] & 0x1;
                    V[instr.x] >>= 1;
                    V[0xF] = lsb;
                    break;
                }
                case 0x7:
                {
                    int8_t notCarry = V[instr.x] <= V[instr.y] ? 1 : 0;
                    V[instr.x] = V[instr.y] - V[instr.x];
                    V[0xF] = notCarry;
                    break;
                }
                case 0xE:
                {
                    int8_t msb = (V[instr.x] >> 7) & 0x01;
                    V[instr.x] <<= 1;
                    V[0xF] = msb;
                    break;
                }
            }
            break;
        }
        case 0x9000:
            // skip next instruction if V[x] != V[y]
            if(V[instr.x] != V[instr.y]) PC += 2;
            break;
        case 0xA000: I = instr.nnn; break;
        case 0xB000: PC = instr.nnn + V[0]; break;
        case 0xC000:
            V[instr.x] = randByte(randGen) & instr.nn;
            break;
        case 0xD000:
        {
            // xors n*8 sprite to video
            uint8_t startCol = V[instr.x];
            uint8_t startRow = V[instr.y];
            uint8_t spriteHeight = instr.n;
            V[0xF] = 0; // V[F] is collision flag (1 xor 1 is done on video), reset it

            for(uint8_t row = 0; row < spriteHeight; ++row)
            {
                uint8_t spriteByte = memory[I + row];
                for(uint8_t col = 0; col < 8; ++col)
                {
                    uint8_t pixel = (spriteByte >> (7 - col)) & 0x1;
                    uint8_t putRow = (startRow + row) & 31; // wrap index
                    uint8_t putCol = (startCol + col) & 63;
                    uint16_t idx = putRow * 64 + putCol;
                    V[0xF] |= video[idx] & pixel;
                    video[idx] ^= pixel;
                }
            }
            break;
        }
        case 0xE000:
        {
            uint8_t key = V[instr.x] & 0xF; // for safety
            switch(instr.nn)
            {
                case 0x9E: if(keypad & (1 << key)) PC += 2; break;
                case 0xA1: if(!(keypad & (1 << key))) PC += 2; break;
            }
            break;
        }
        case 0xF000:
        {

            switch(instr.nn)
            {
                case 0x07: V[instr.x] = DT; break;
                case 0x0A:
                {
                    // pause execution till a keypress detected
                    if(keypad == 0)
                    {
                        PC -= 2;
                        break;
                    }
                    for(uint8_t i = 0x0; i <= 0xF; ++i)
                    {
                        if(keypad & (1 << i))
                        {
                            V[instr.x] = i;
                            break;
                        }
                    }
                    break;
                }
                case 0x15: DT = V[instr.x]; break;
                case 0x18: ST = V[instr.x]; break;
                case 0x1E: I = I + V[instr.x]; break;
                case 0x29:
                {
                    uint8_t digit = V[instr.x] & 0xF;
                    I = fontStart + digit * 5; 
                    break;
                }
                case 0x33:
                {
                    // separates 3 decimal digits of decimal representation of V[x] in memory for video
                    if(I+2 > 0xFFF) Error("Memory Access Violation");
                    memory[I] = V[instr.x] / 100;
                    memory[I+1] = (V[instr.x] / 10) % 10;
                    memory[I+2] = V[instr.x] % 10;
                    break;
                }
                case 0x55:
                {
                    if(I + instr.x > 0xFFF) Error("Memory Access Violation");
                    memcpy(&memory[I], &V[0], instr.x+1);
                    break;
                }
                case 0x65:
                {
                    if(I + instr.x > 0xFFF) Error("Memory Access Violation");
                    memcpy(&V[0], &memory[I], instr.x+1);
                    break;
                }
            }
            break;
        }
    }
}

#include <cstddef>
#include <iostream>
#include <fstream>
#include <stdio.h>

#define DISPLAY_W 64
#define DISPLAY_H 32
#define RAM_START 0x000
#define PROGRAM_START 0x200
#define ETI_START 0x600
#define RAM_END 0xFFF
#define RESET "\033[0m"
#define RED "\033[31m"

unsigned char VRAM[4096];
unsigned char DMem[64 * 32];

unsigned char font[0x10 * 0x5] = {0xF0, 0x90, 0x90, 0x90, 0xF0,  //0
                                  0x20, 0x60, 0x20, 0x20, 0x70,  //1
                                  0xF0, 0x10, 0xF0, 0x80, 0xF0,  //2
                                  0xF0, 0x10, 0xF0, 0x10, 0xF0,  //3
                                  0x90, 0x90, 0xF0, 0x10, 0x10,  //4
                                  0xF0, 0x80, 0xF0, 0x10, 0xF0,  //5
                                  0xF0, 0x80, 0xF0, 0x90, 0xF0,  //6
                                  0xF0, 0x10, 0x20, 0x40, 0x40,  //7
                                  0xF0, 0x90, 0xF0, 0x90, 0xF0,  //8
                                  0xF0, 0x90, 0xF0, 0x10, 0xF0,  //9
                                  0xF0, 0x90, 0xF0, 0x90, 0x90,  //A
                                  0xE0, 0x90, 0xE0, 0x90, 0xE0,  //B
                                  0xF0, 0x80, 0x80, 0x80, 0xF0,  //C
                                  0xE0, 0x90, 0x90, 0x90, 0xE0,  //D
                                  0xF0, 0x80, 0xF0, 0x80, 0xF0,  //E
                                  0xF0, 0x80, 0xF0, 0x80, 0x80}; //F


const std::string getByteString(unsigned char number)
{
    unsigned char firstNibble = number >> 4 & 0xF;
    unsigned char secondNibble = number & 0xF0 >> 4;
    std::string nibble1(sizeof(const char), "0123456789ABCDEF"[firstNibble]);
    std::string nibble2(sizeof(const char), "0123456789ABCDEF"[secondNibble]);
    return nibble1 + nibble2;
}

const std::string getByteString(unsigned short int number)
{
    unsigned char firstNibble = number >> 12;
    unsigned char secondNibble = number >> 8 & 0x0F;
    unsigned char thirdNibble = number >> 4 & 0xF;
    unsigned char fourthNibble = number & 0xF;

    std::string nibble1(sizeof(const char), "0123456789ABCDEF"[firstNibble]);
    std::string nibble2(sizeof(const char), "0123456789ABCDEF"[secondNibble]);
    std::string nibble3(sizeof(const char), "0123456789ABCDEF"[thirdNibble]);
    std::string nibble4(sizeof(const char), "0123456789ABCDEF"[fourthNibble]);
    return nibble1 + nibble2 + nibble3 + nibble4;
}

union Opcode {
    unsigned short int inst;
    union {
        unsigned short int nnn : 12;
        struct
        {
            unsigned char n : 4;
            unsigned char y : 4;
            unsigned char x : 4;
            unsigned char u : 4;
        };
        unsigned char kk;
    };
};

class Chip8
{
public:
    unsigned char V[16], DT, ST, SP;
    unsigned short int I, PC, STACK[16];
    unsigned char fonts[0x10 * 0x5];

    void inst_0nnn() {}
    void inst_00E0() { memset(DMem, 0x0, sizeof(DMem)); }
    void inst_00EE() { PC = STACK[SP]; SP--; }

    void inst_1nnn(Opcode opcode) { PC = opcode.nnn; }
    void inst_2nnn(Opcode opcode) { STACK[++SP] = PC; PC = opcode.nnn; }
    void inst_3xkk(Opcode opcode) { if (V[opcode.x] == opcode.kk) PC += 2; }
    void inst_4xkk(Opcode opcode) { if (V[opcode.x] != opcode.kk) PC += 2; }
    void inst_5xy0(Opcode opcode) { if (V[opcode.x] == V[opcode.y]) PC += 2; }

    void inst_6xkk(Opcode opcode) { V[opcode.x] = opcode.kk; }
    void inst_7xkk(Opcode opcode) { V[opcode.x] += opcode.kk; }

    void inst_8xy0(Opcode opcode) { V[opcode.x] = V[opcode.y]; }
    void inst_8xy1(Opcode opcode) { V[opcode.x] |= V[opcode.y]; }
    void inst_8xy2(Opcode opcode) { V[opcode.x] &= V[opcode.y]; }
    void inst_8xy3(Opcode opcode) { V[opcode.x] ^= V[opcode.y]; }
    void inst_8xy4(Opcode opcode) { unsigned short int sum = V[opcode.x] + V[opcode.y]; V[0xF] = (sum > 0xFF) ? 1 : 0; V[opcode.x] = sum & 0xFF; }
    void inst_8xy5(Opcode opcode) { V[0xF] = (V[opcode.x] > V[opcode.y]) ? 1 : 0; V[opcode.x] -= V[opcode.y]; }
    void inst_8xy6(Opcode opcode) { V[0xF] = V[opcode.x] & 1; V[opcode.x] = V[opcode.y] >> 1; }
    void inst_8xy7(Opcode opcode) { V[0xF] = (V[opcode.y] > V[opcode.x]) ? 1 : 0; V[opcode.x] = V[opcode.y] - V[opcode.x]; }
    void inst_8xyE(Opcode opcode) { V[0xF] = V[opcode.x] >> 7; V[opcode.x] = V[opcode.y] << 1; }

    void inst_9xy0(Opcode opcode) { if (V[opcode.y] != V[opcode.x]) PC += 2; }

    void inst_Annn(Opcode opcode) { I = opcode.nnn; }
    void inst_Bnnn(Opcode opcode) { PC = opcode.nnn + V[0]; }
    void inst_Cxkk(Opcode opcode) { int randomNumber = rand() % 256; V[opcode.x] = (randomNumber & opcode.kk); }

    void inst_Dxyn(Opcode opcode)
    {
        unsigned char vx = V[opcode.x] % DISPLAY_W;
        unsigned char vy = V[opcode.y] % DISPLAY_H;
        V[0xF] = 0x0;
        for(int index=0; index < opcode.n; index++){
            unsigned short int spriteVy = ((vy + index) * DISPLAY_W);
            unsigned char newByte = VRAM[I+index];

            for(int pos=0; pos<8; pos++){
                unsigned short int spriteIndex = ((vx + pos) % DISPLAY_W) + spriteVy;
                unsigned char shiftSize = (7 - pos % 8);
                unsigned char newBit = (newByte >> shiftSize) & 1;
                unsigned char oldBit = DMem[spriteIndex] & 1;
                V[0xF] |= !oldBit ? 0 : oldBit ^ newBit;
                DMem[spriteIndex] = oldBit ^ newBit;
            }
        }
    }

    void inst_Ex9E(Opcode opcode) { /*have to implement */ PC += 2;}
    void inst_ExA1(Opcode opcode) { /*have to implement*/ PC += 2; }
    void inst_Fx07(Opcode opcode) { V[opcode.x] = DT; }
    void inst_Fx0A(Opcode opcode) { /*have to implement*/ }
    void inst_Fx15(Opcode opcode) { DT = V[opcode.x]; }
    void inst_Fx18(Opcode opcode) { ST = V[opcode.x]; }
    void inst_Fx1E(Opcode opcode) { I += V[opcode.x]; V[0xF] = I > 0xFFF ? 1 : 0; }
    void inst_Fx29(Opcode opcode) { I = &VRAM[V[opcode.x] * 0x5] - VRAM; }

    void inst_Fx33(Opcode opcode)
    {
        uint8_t decimalValue = V[opcode.x];
        VRAM[I] = decimalValue / 100;
        VRAM[I + 1] = (decimalValue % 100) / 10;
        VRAM[I + 2] = decimalValue % 10;
    }

    void inst_Fx55(Opcode opcode)
    {
        for (int i = 0; i <= opcode.x; i++)
        {
            VRAM[I + i] = V[i];
        }
    }
    void inst_Fx65(Opcode opcode)
    {
        for (int i = 0; i <= opcode.x; i++)
        {
            V[i] = VRAM[I + i];
        }
    }

    void inst_0xxx(Opcode opcode)
    {
        switch(opcode.kk){
            case 0xE0: inst_00E0(); break;
            case 0xEE: inst_00EE(); break;
        }
    }

    void inst_8xxx(Opcode opcode)
    {
        switch(opcode.n){
            case 0x0: inst_8xy0(opcode); break;
            case 0x1: inst_8xy1(opcode); break;
            case 0x2: inst_8xy2(opcode); break;
            case 0x3: inst_8xy3(opcode); break;
            case 0x4: inst_8xy4(opcode); break;
            case 0x5: inst_8xy5(opcode); break;
            case 0x6: inst_8xy6(opcode); break;
            case 0x7: inst_8xy7(opcode); break;
            case 0xE: inst_8xyE(opcode); break;
        }
    }

    void inst_Exxx(Opcode opcode)
    {
        switch(opcode.kk){
            case 0x9E: inst_Ex9E(opcode); break;
            case 0xA1: inst_ExA1(opcode); break;
        }
    }

    void inst_Fxxx(Opcode opcode)
    {
        switch(opcode.kk){
            case 0x15: inst_Fx15(opcode); break;
            case 0x18: inst_Fx18(opcode); break;
            case 0x1E: inst_Fx1E(opcode); break;
            case 0x29: inst_Fx29(opcode); break;
            case 0x33: inst_Fx33(opcode); break;
            case 0x55: inst_Fx55(opcode); break;
            case 0x65: inst_Fx65(opcode); break;
        }
    }

    void executeInstruction(Opcode opcode)
    {
        switch(opcode.u){
            case 0x0: inst_0xxx(opcode); break;
            case 0x1: inst_1nnn(opcode); break;
            case 0x2: inst_2nnn(opcode); break;
            case 0x3: inst_3xkk(opcode); break;
            case 0x4: inst_4xkk(opcode); break;
            case 0x5: inst_5xy0(opcode); break;
            case 0x6: inst_6xkk(opcode); break;
            case 0x7: inst_7xkk(opcode); break;
            case 0x8: inst_8xxx(opcode); break;
            case 0x9: inst_9xy0(opcode); break;
            case 0xA: inst_Annn(opcode); break;
            case 0xB: inst_Bnnn(opcode); break;
            case 0xC: inst_Cxkk(opcode); break;
            case 0xD: inst_Dxyn(opcode); break;
            case 0xE: inst_Exxx(opcode); break;
            case 0xF: inst_Fxxx(opcode); break;
            default: std::cout << "No instruction found." << std::endl;
        }
    }

    void loadFile(std::string fileName, unsigned char *buffer)
    {
        std::ifstream fileStream;
        for (fileStream.open(fileName, std::ios::binary); fileStream.good();)
        {
            *buffer = (unsigned char) fileStream.get();
            buffer++;
        }
    };

    Opcode readInstruction(unsigned short int address)
    {
        Opcode opcode;
        opcode.inst = (VRAM[address] << 8) | VRAM[address + 1];
        this->PC+=2;
        return opcode;
    }

    void render(unsigned char *dispBuffer){
        std:system("clear");
        for(int h=0; h < DISPLAY_H; h++){
            for(int w=0; w < DISPLAY_W; w++){
                if(dispBuffer[w + (h * DISPLAY_W)]) std::cout << "*";
                else std::cout << " ";
            }

            std::cout << "           ";
            //Beginning displaying registers
            if(h == 0){
                for (int index = 0; index < 16; index++)
                {
                    std::cout << "V[" << std::hex << int(index) << "] ";
                }

                std::cout << "DT" << "  " << "ST" << "  " << "I" << "     " << "PC";
            }

            if(h == 1){
                for (int index = 0; index < 16; index++)
                {
                    std::string byte = getByteString(V[index]);
                    std::cout << byte << "   ";
                }

                std::cout << std::hex << getByteString(DT) << "  ";
                std::cout << std::hex << getByteString(ST) << "  ";
                std::cout << getByteString(I) << "  ";
                std::cout << getByteString(PC);
            }
            //End displaying registers

            if(h == 4) {
                for (int w = 0; w < 64; w += 2)
                {
                    std::cout << std::hex << int(w + ((h - 4) * DISPLAY_W)) + 0x200 << "  ";
                }
            }

            //Beginning displaying memory
            if(h >= 5 && h < 25){
                int height = h - 5;
                for (int w = 0; w < 64; w += 2)
                {
                    unsigned short int byte1_addr = (w + (height * DISPLAY_W)) + PROGRAM_START;
                    unsigned short int byte2_addr = (w + 1 + (height * DISPLAY_W)) + PROGRAM_START;
                    unsigned char byte1 = VRAM[byte1_addr];
                    unsigned char byte2 = VRAM[byte2_addr];

                    std::string byteString1 = getByteString(byte1);
                    std::string byteString2 = getByteString(byte2);
                    
                    byte1_addr == PC ? std::cout << RED << byteString1 << RESET : std::cout << byteString1;
                    byte2_addr == PC ? std::cout << RED << byteString2 << RESET : std::cout << byteString2;
                    std::cout << " ";
                }
            }

            std::cout << std::endl;
        }
    }

public:
    void setup(std::string fileName)
    {
        this->PC = PROGRAM_START;
        loadFile(fileName, &VRAM[this->PC]);
        std::memcpy(&VRAM[0x0], font, sizeof(font));
    };

    void mainLoop()
    {
        for (; this->PC <= RAM_END;)
        {
            Opcode opcode = this->readInstruction(this->PC);
            this->executeInstruction(opcode);
            this->render(DMem);
            if(this->DT > 0) this->DT -= 1;
            if(this->ST > 0) { this->ST -= 1; }
            //getchar();
        }
    };
} chip8;


int main(int argc, char *argv[])
{
    chip8.setup(argv[1]);
    chip8.mainLoop();
    return 0;
}

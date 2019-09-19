#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <string>
using namespace std;

#define INIT_RESERVED_INTERPRETER 0x000;
#define END_RESERVED_INTERPRETER 0x1FF;
#define PROGRAM_START 0x200;
#define ETI_PROGRAMS 0x600;
#define INITIAL_LOCATION 0xFFF;
#define W 64
#define H 32

union Opcode { 
    uint16_t inst;
    union{
        uint16_t nnn: 12;
        struct {
            uint8_t n: 4;
            uint8_t y: 4;
            uint8_t x: 4;
            uint8_t u: 4;
        };
        uint8_t kk;
    };
};

uint8_t VRAM[4096], DisplayMemory[W * H] = {0x0}, SP;
uint16_t STACK[16];

class CPU{
    public:
        uint8_t V[16], DT, ST, VF;
        uint16_t I, PC;

        uint16_t readOpcode(uint8_t *memory){
            return (memory[PC] << 8) + memory[PC+1]; 
        }

        //Not used in modern interpreters
        void inst_0nnn(){}

        //Clear display
        void inst_00E0(Opcode opcode){
            memset(DisplayMemory, 0x0, W * H);
        }

        void inst_00EE(Opcode opcode){
            PC = STACK[SP];
            SP--;
        }

        void inst_1nnn(Opcode opcode){
            PC = opcode.nnn;
        }

        void inst_2nnn(Opcode opcode){
            STACK[++SP] = PC;
            PC = opcode.nnn;
        }

        void inst_3xkk(Opcode opcode){
            if(V[opcode.x] == opcode.kk) PC+=2;
        }

        void inst_4xkk(Opcode opcode){
            if(V[opcode.x] != opcode.kk) PC+=2;
        }

        void inst_5xy0(Opcode opcode){
            if(V[opcode.x] == V[opcode.y]) PC +=2;
        }

        void inst_6xkk(Opcode opcode){
            V[opcode.x] = opcode.kk;
        }

        void inst_7xkk(Opcode opcode){
            V[opcode.x] += opcode.kk;
        }

        void inst_8xy0(Opcode opcode){
            V[opcode.y] = V[opcode.x];
        }

        void inst_8xy1(Opcode opcode){
            V[opcode.x] = V[opcode.y] | V[opcode.x];
        }

        void inst_8xy2(Opcode opcode){
            V[opcode.x] = V[opcode.y] & V[opcode.x];
        }

        void inst_8xy3(Opcode opcode){
            V[opcode.x] = V[opcode.y] ^ V[opcode.x];
        }

        void inst_8xy4(Opcode opcode){
            uint16_t sum = V[opcode.x] + V[opcode.y];
            VF = (sum > 0xFF) ? 1 : 0;
            V[opcode.x] = sum & 0xFF;
        }

        void inst_8xy5(Opcode opcode){
            VF = (V[opcode.y] > V[opcode.x]) ? 1 : 0;
            V[opcode.x] = V[opcode.y] - V[opcode.x];
        }

        void inst_8xy6(Opcode opcode){
            VF = ((V[opcode.x] & 0x01) == 1) ? 1 : 0;
            V[opcode.x] /= 2;
        }

        void inst_8xy7(Opcode opcode){
            VF = (V[opcode.y] > V[opcode.x]) ? 1 : 0;
            V[opcode.x] = V[opcode.x] - V[opcode.y];
        }

        void inst_8xyE(Opcode opcode){
            VF = (V[opcode.x] >> 7) ? 1 : 0;
            V[opcode.x] = V[opcode.x] * 2;
        }

        void inst_9xy0(Opcode opcode){
            if(V[opcode.y] != V[opcode.x]) PC+=2;
        }

        void inst_Annn(Opcode opcode){
            I = opcode.nnn;
        }

        void inst_Bnnn(Opcode opcode){
            PC = opcode.nnn + V[0];
        }

        void inst_Cxkk(Opcode opcode){
            int randomNumber = rand() % 256;
            V[opcode.x] = (randomNumber & opcode.kk); 
        }

        //Display and keyboard related functions
        void inst_Dxyn(Opcode opcode){
            for(int index = 0; index < opcode.n; index++){
                uint8_t byte = VRAM[I + index];
                uint16_t vx = V[opcode.x] % W;
                uint16_t vy = (V[opcode.y] % H) * W;
                uint16_t spriteIndex = (vx + vy + index) % (W * H);
                uint8_t oldByte = DisplayMemory[spriteIndex];
                VF = (byte ^ oldByte) != oldByte ? 1 : 0;
                DisplayMemory[spriteIndex] = byte;
            }
        }

        void inst_Ex9E(Opcode opcode){}
        void inst_ExA1(Opcode opcode){}
        void inst_Fx07(Opcode opcode){}
        void inst_Fx0A(Opcode opcode){}
        void inst_Fx15(Opcode opcode){}
        void inst_Fx18(Opcode opcode){}
        void inst_Fx1E(Opcode opcode){}
        void inst_Fx29(Opcode opcode){}
        void inst_Fx33(Opcode opcode){}
        void inst_Fx55(Opcode opcode){}
        void inst_Fx65(Opcode opcode){}

        void inst_0xxx(Opcode opcode){
            switch(opcode.kk){
                case 0xE0: inst_00E0(opcode); break;
                case 0xEE: inst_00EE(opcode); break;
            }
        }
        void inst_8xxx(Opcode opcode){
            switch(opcode.n){
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

        void inst_Exxx(Opcode opcode){
            switch(opcode.kk){
                case 0x9E: inst_Ex9E(opcode); break;
                case 0xA1: inst_ExA1(opcode); break;
            }
        }

        void inst_Fxxx(Opcode opcode){
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

        void execute(Opcode opcode){
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
                default: cout << "No instruction found." << endl;
            }
        }
};

void LoadFile(string filename, uint8_t* buffer){
    ifstream file;
    for(file.open(filename, ios::binary); file.good(); ){ *buffer = file.get(); cout << hex << int(*buffer) ; buffer++;}
}

int main(int argc, char* argv[]){
    CPU cpu;
    cpu.PC = 0x200;
    //LoadFile("test_opcode.ch8", &VRAM[0x200]);
    Opcode opcode;
    cpu.V[0x0] = 0x3F;
    cpu.V[0x1] = 0x1F;

    VRAM[0x1] = 0xAA;
    VRAM[0x2] = 0xAA;
    VRAM[0x3] = 0xAA;
    VRAM[0x4] = 0xAA;
    VRAM[0x5] = 0xAA;
    VRAM[0x6] = 0xAA;
    VRAM[0x7] = 0xAA;
    VRAM[0x8] = 0xAA;
    VRAM[0x9] = 0xAA;
    VRAM[0xA] = 0xAA;
    VRAM[0xB] = 0xAA;
    VRAM[0xC] = 0xAA;
    VRAM[0xD] = 0xAA;
    VRAM[0xE] = 0xAA;
    VRAM[0xF] = 0xAA;

    VRAM[cpu.PC] = 0xA0;
    VRAM[cpu.PC + 1] = 0x1;
    opcode.inst = cpu.readOpcode(VRAM);
    cpu.execute(opcode);
    VRAM[cpu.PC + 2] = 0xD0;
    VRAM[cpu.PC + 3] = 0x1F;
    cpu.PC += 2;
    opcode.inst = cpu.readOpcode(VRAM);
    cpu.execute(opcode);


    for(int i = 0; i < H * W; i++){
            cout << hex << int(DisplayMemory[i]) ;
    }

    return 0;
}

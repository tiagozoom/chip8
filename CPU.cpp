#include "cpu.h"

using namespace std;

void CPU::Init(void){
    PC = 0x200; 
    SP = DT = ST = I = PC = 0;
    memset(V, 0x0, 16 * sizeof(uint8_t));
}

uint16_t CPU::read(uint8_t *memory){
    uint16_t opcode = (memory[PC] << 8) + memory[PC+1]; 
    PC+=2;
    return opcode;
}

void CPU::inst_0nnn(){}

void CPU::inst_00E0(){
    memset(pixels, 0x0, DISPLAY_SIZE * sizeof(Uint32));
}

void CPU::inst_00EE(){
    PC = STACK[SP];
    SP--;
}

void CPU::inst_1nnn(Opcode opcode){
    PC = opcode.nnn;
}

void CPU::inst_2nnn(Opcode opcode){
    STACK[++SP] = PC;
    PC = opcode.nnn;
}

void CPU::inst_3xkk(Opcode opcode){
    if(V[opcode.x] == opcode.kk) PC+=2;
}

void CPU::inst_4xkk(Opcode opcode){
    if(V[opcode.x] != opcode.kk) PC+=2;
}

void CPU::inst_5xy0(Opcode opcode){
    if(V[opcode.x] == V[opcode.y]) PC +=2;
}

void CPU::inst_6xkk(Opcode opcode){
    V[opcode.x] = opcode.kk;
}

void CPU::inst_7xkk(Opcode opcode){
    V[opcode.x] += opcode.kk;
}

void CPU::inst_8xy0(Opcode opcode){
    V[opcode.x] = V[opcode.y];
}

void CPU::inst_8xy1(Opcode opcode){
    V[opcode.x] = V[opcode.y] | V[opcode.x];
}

void CPU::inst_8xy2(Opcode opcode){
    V[opcode.x] = V[opcode.y] & V[opcode.x];
}

void CPU::inst_8xy3(Opcode opcode){
    V[opcode.x] = V[opcode.y] ^ V[opcode.x];
}

void CPU::inst_8xy4(Opcode opcode){
    uint16_t sum = V[opcode.x] + V[opcode.y];
    V[0xF] = (sum > 0xFF) ? 1 : 0;
    V[opcode.x] = sum & 0xFF;
}

void CPU::inst_8xy5(Opcode opcode){
    V[0xF] = (V[opcode.x] > V[opcode.y]) ? 1 : 0;
    V[opcode.x] = V[opcode.x] - V[opcode.y];
}

void CPU::inst_8xy6(Opcode opcode){
    V[0xF] = ((V[opcode.x] & 1) == 1) ? 1 : 0;
    V[opcode.x] /= 2;
}

void CPU::inst_8xy7(Opcode opcode){
    V[0xF] = (V[opcode.y] > V[opcode.x]) ? 1 : 0;
    V[opcode.x] = V[opcode.x] - V[opcode.y];
}

void CPU::inst_8xyE(Opcode opcode){
    V[0xF] = (V[opcode.x] >> 7) ? 1 : 0;
    V[opcode.x] = V[opcode.x] * 2;
}

void CPU::inst_9xy0(Opcode opcode){
    if(V[opcode.y] != V[opcode.x]) PC+=2;
}

void CPU::inst_Annn(Opcode opcode){
    I = opcode.nnn;
}

void CPU::inst_Bnnn(Opcode opcode){
    PC = opcode.nnn + V[0];
}

void CPU::inst_Cxkk(Opcode opcode){
    int randomNumber = rand() % 256;
    V[opcode.x] = (randomNumber & opcode.kk); 
}

//Display and keyboard related functions
void CPU::inst_Dxyn(Opcode opcode){
    uint8_t vx = V[opcode.x] % W;
    for(int index = 0; index < opcode.n; index++){
        uint8_t byte = chip8.VRAM[I + index];
        uint16_t vy = ((V[opcode.y] % H) + index) * W;
        uint16_t spriteIndex = vx + vy;
        for(int pos=0; pos<8; pos++){
            uint8_t bit = (7 - pos % 8);
            Uint32  newPixel = ((byte >> bit) & 0x1) ? 0xFFFFFFFF : 0xFF000000;
            V[0xF] |= (newPixel != pixels[spriteIndex + pos]) ? 1 : 0;
            pixels[spriteIndex + pos] = newPixel;
        }

    }
}

void CPU::inst_Ex9E(Opcode opcode){
    //have to implement
    PC += 2; 
}
void CPU::inst_ExA1(Opcode opcode){
    //have to implement
    PC += 2; 
}
void CPU::inst_Fx07(Opcode opcode){
    V[opcode.x] = DT; 
}
void CPU::inst_Fx0A(Opcode opcode){
    //have to implement
}
void CPU::inst_Fx15(Opcode opcode){
    DT = V[opcode.x];
}
void CPU::inst_Fx18(Opcode opcode){
    ST = V[opcode.x];
}
void CPU::inst_Fx1E(Opcode opcode){
    I += V[opcode.x];
    V[0xF] = I > 0xFFF ? 1 : 0;
}
void CPU::inst_Fx29(Opcode opcode){
    I = chip8.VRAM - &chip8.font[V[opcode.x]];
}
void CPU::inst_Fx33(Opcode opcode){
    uint8_t decimalValue = V[opcode.x];
    chip8.VRAM[I] = decimalValue / 100;
    chip8.VRAM[I+1] = (decimalValue % 100) / 10;
    chip8.VRAM[I+2] = decimalValue % 10;
}
void CPU::inst_Fx55(Opcode opcode){
    for(int i = 0; i <= opcode.x; i++){
        chip8.VRAM[I + i] = V[i];
    }
}
void CPU::inst_Fx65(Opcode opcode){
    for(int i = 0; i <= opcode.x; i++){
        V[i] = chip8.VRAM[I + i];
    }
}

void CPU::inst_0xxx(Opcode opcode){
    switch(opcode.kk){
        case 0xE0: inst_00E0(); break;
        case 0xEE: inst_00EE(); break;
    }
}
void CPU::inst_8xxx(Opcode opcode){
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

void CPU::inst_Exxx(Opcode opcode){
    switch(opcode.kk){
        case 0x9E: inst_Ex9E(opcode); break;
        case 0xA1: inst_ExA1(opcode); break;
    }
}

void CPU::inst_Fxxx(Opcode opcode){
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

void CPU::execute(Opcode opcode){
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

#include <iostream> 
#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <string>
#include "display_controller.h" 
#include "cpu.h" 
#include "chip8.h"

using namespace std;

#define INIT_RESERVED_INTERPRETER 0x000
#define END_RESERVED_INTERPRETER 0x1FF
#define PROGRAM_START 0x200
#define ETI_PROGRAMS 0x600
#define INITIAL_LOCATION 0xFFF
#define RESET "\033[0m"
#define RED "\033[31m"

Uint32 pixels[DISPLAY_SIZE];
Chip8 chip8;

void LoadFile(string filename, uint8_t* buffer){
    ifstream file;
    for(file.open(filename, ios::binary); file.good(); ){ 
        *buffer = file.get();  
        buffer++;
    }
}

const string getByteString(uint8_t number){
        uint8_t firstNibble  = number >> 4 & 0xF;
        uint8_t secondNibble = number & 0xF0 >> 4;
        string nibble1(sizeof(const char), "0123456789ABCDEF"[firstNibble]);
        string nibble2(sizeof(const char), "0123456789ABCDEF"[secondNibble]);
        return nibble1 + nibble2; 
}

const string getByteString(uint16_t number){
        uint8_t firstNibble  = number >> 12;
        uint8_t secondNibble  = number >> 8 & 0x0F;
        uint8_t thirdNibble = number >> 4 & 0xF;
        uint8_t fourthNibble = number  & 0xF;
        
        string nibble1(sizeof(const char), "0123456789ABCDEF"[firstNibble]);
        string nibble2(sizeof(const char), "0123456789ABCDEF"[secondNibble]);
        string nibble3(sizeof(const char), "0123456789ABCDEF"[thirdNibble]);
        string nibble4(sizeof(const char), "0123456789ABCDEF"[fourthNibble]);
        return nibble1 + nibble2 + nibble3 + nibble4; 
}

void DisplayRegisters(CPU cpu){
    for(int index=0; index<16; index++){
        cout << "V[" << hex << int(index) << "] ";
    }

    cout << endl;

    for(int index=0; index<16; index++){
        string byte = getByteString(cpu.V[index]);
        cout << byte << "   ";
    }

    cout << endl << endl << "DT" << "  " << "ST" << "  " << "VF" << "  " << "I" << "     " << "PC" << endl;
    cout << hex << getByteString(cpu.DT) << "  ";
    cout << hex << getByteString(cpu.ST) << "  ";
    cout << hex << getByteString(cpu.V[0xF]) << "  ";
    cout << getByteString(cpu.I) << "  ";
    cout << getByteString(cpu.PC) << endl;
}

void DisplayConsoleMem(uint8_t* VRAM, uint16_t current_address){
    cout << endl << endl;
    for(int h=0; h<8; h++){
        for(int w=0; w<64; w+=2){
            if((w + (h *  W)) % 16 == 0) cout << hex << int(w + (h *  W)) + 0x200<< endl;
            uint8_t byte1 = int(VRAM[w + (h *  W)]); string byteString1 = getByteString(byte1);
            uint8_t byte2 = int(VRAM[w+1 + (h *  W)]);
            string byteString2 = getByteString(byte2);
            ((w + (h * W)) == (current_address - PROGRAM_START)) ? cout << RED <<  byteString1 << byteString2 << RESET << " " : cout <<  byteString1 << byteString2 << " ";
        }
        cout << endl;
    }
}

bool debug(){
    for(SDL_Event event; SDL_WaitEvent(&event);){
        if(event.type == SDL_QUIT) { return true;}
        if(event.type == SDL_KEYUP) { break; }
    }
    
    return false;
}

bool nonDebug(){
    for(SDL_Event event; SDL_PollEvent(&event);){
        if(event.type == SDL_QUIT) { return true; }
    }

    return false;
}

int main(int argc, char* argv[]){
    uint8_t font_test[0x10*0x5] = { 0xF0, 0x90, 0x90, 0x90, 0xF0, //0
                                   0x20, 0x60, 0x20, 0x20, 0x70, //1
                                   0xF0, 0x10, 0xF0, 0x80, 0xF0, //2 
                                   0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
                                   0x90, 0x90, 0xF0, 0x10, 0x10,  //4
                                   0xF0, 0x80, 0xF0, 0x10, 0xF0,  //5
                                   0xF0, 0x80, 0xF0, 0x90, 0xF0,  //6
                                   0xF0, 0x10, 0x20, 0x40, 0x40, //7
                                   0xF0, 0x90, 0xF0, 0x90, 0xF0,  //8
                                   0xF0, 0x90, 0xF0, 0x10, 0xF0,  //9
                                   0xF0, 0x90, 0xF0, 0x90, 0x90,  //A
                                   0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
                                   0xF0, 0x80, 0x80, 0x80, 0xF0, //C
                                   0xE0, 0x90, 0x90, 0x90, 0xE0, //D
                                   0xF0, 0x80, 0xF0, 0x80, 0xF0,  //E
                                   0xF0, 0x80, 0xF0, 0x80, 0x80 }; //F
    memcpy(chip8.font, font_test, (0x10*0x5)*sizeof(uint8_t));
    //cout << hex << ( &chip8.font[0x6 * 0x5] - chip8.VRAM) << endl;
    CPU cpu;
    cpu.Init();
    chip8.display.Init();
    Opcode opcode;
    cpu.PC = 0x200;
    LoadFile(argv[1], &chip8.VRAM[cpu.PC]);
    uint8_t* pointer = &chip8.VRAM[cpu.PC];

    bool (*loopPointer)() = (argv[2] && strcmp(argv[2], "debug") == 0) ? debug : nonDebug;
    bool quit = false;

    while(!quit){
        opcode.inst = cpu.read(chip8.VRAM);
        cpu.execute(opcode);
        DisplayRegisters(cpu);
        DisplayConsoleMem(&chip8.VRAM[PROGRAM_START], cpu.PC);
        chip8.display.show(pixels);
        /*cout << "Whole instruction: " << hex << opcode.inst << endl;
        cout << "Instruction: " << hex << opcode.nnn;
        cout << " V1 " <<  hex << int(cpu.V[0x1]);
        cout << " V2 " <<  hex << int(cpu.V[0x2]);
        cout << " V3 " <<  hex << int(cpu.V[0x3]);
        cout << " V4 " <<  hex << int(cpu.V[0x4]);
        cout << " V5 " <<  hex << int(cpu.V[0x5]);
        cout << " V6 " <<  hex << int(cpu.V[0x6]);
        cout << " V7 " <<  hex << int(cpu.V[0x7]);
        cout << " V8 " <<  hex << int(cpu.V[0x8]);
        cout << " V9 " <<  hex << int(cpu.V[0x9]);
        cout << " VA " <<  hex << int(cpu.V[0xA]);
        cout << " VB " <<  hex << int(cpu.V[0xB]);
        cout << " VC " <<  hex << int(cpu.V[0xC]);
        cout << " VD " <<  hex << int(cpu.V[0xD]);
        cout << " VE " <<  hex << int(cpu.V[0xE]);
        cout << " VF " <<  hex << int(cpu.V[0xF]) << endl;*/

        pointer+=2;
        quit = loopPointer();
        
        cpu.checkDelayTimer();
        //system("clear"); 
    }

    return 0;
}

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <string>
#include "display_controller.h" 
#include "CPU.h" 

using namespace std;

#define INIT_RESERVED_INTERPRETER 0x000;
#define END_RESERVED_INTERPRETER 0x1FF;
#define PROGRAM_START 0x200;
#define ETI_PROGRAMS 0x600;
#define INITIAL_LOCATION 0xFFF;
#define RESET "\033[0m"
#define RED "\033[31m"

uint8_t VRAM[4096];
Uint32 pixels[DISPLAY_SIZE];

void LoadFile(string filename, uint8_t* buffer){
    ifstream file;
    for(file.open(filename, ios::binary); file.good(); ){ *buffer = file.get();  buffer++;}
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
    cout << hex << getByteString(cpu.VF) << "  ";
    cout << getByteString(cpu.I) << "  ";
    cout << getByteString(cpu.PC) << endl;
}

void DisplayConsoleMem(uint8_t* VRAM, uint16_t current_address){
    cout << endl << endl;
    for(int h=0; h<52; h++){
        for(int w=0; w<64; w+=2){
            uint8_t byte1 = int(VRAM[w + (h *  W)]);
            string byteString1 = getByteString(byte1);
            uint8_t byte2 = int(VRAM[w+1 + (h *  W)]);
            string byteString2 = getByteString(byte2);
            ((w + (h * W)) == (current_address - 0x200)) ? cout << RED <<  byteString1 << byteString2 << RESET << " " : cout <<  byteString1 << byteString2 << " ";
        }
        cout << endl;
    }
}

int main(int argc, char* argv[]){
    CPU cpu;
    Opcode opcode;
    DisplayController display;
    cpu.PC = 0x200;
    LoadFile(argv[1], &VRAM[cpu.PC]);
    uint8_t* pointer = &VRAM[cpu.PC];
    
    do {
        system("clear"); 

        DisplayRegisters(cpu);
        DisplayConsoleMem(&VRAM[0x200], cpu.PC);

        opcode.inst = cpu.read(VRAM);
        cpu.execute(opcode);

        cout << endl << "Instruction: " << opcode.inst << endl;
        display.show(pixels);

        pointer+=2;
    } while(cin.get() != 27);

    display.stop();
    return 0;
}

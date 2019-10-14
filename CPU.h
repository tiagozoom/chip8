#include <SDL2/SDL.h>
#include "chip8.h"
#include "display.h"
#include "opcode.h" 

class CPU {
    public:
        void Init();
        uint16_t STACK[16];
        uint8_t V[16], DT, ST, SP;
        uint16_t I, PC;

        uint16_t read(uint8_t *memory);

        //Not used in modern interpreters
        void inst_0nnn();

        //Clear display
        void inst_00E0();
        void inst_00EE();

        //General Instructions
        void inst_1nnn(Opcode opcode);
        void inst_2nnn(Opcode opcode);
        void inst_3xkk(Opcode opcode);
        void inst_4xkk(Opcode opcode);
        void inst_5xy0(Opcode opcode);
        void inst_6xkk(Opcode opcode);
        void inst_7xkk(Opcode opcode);
        void inst_8xy0(Opcode opcode);
        void inst_8xy1(Opcode opcode);
        void inst_8xy2(Opcode opcode);
        void inst_8xy3(Opcode opcode);
        void inst_8xy4(Opcode opcode);
        void inst_8xy5(Opcode opcode);
        void inst_8xy6(Opcode opcode);
        void inst_8xy7(Opcode opcode);
        void inst_8xyE(Opcode opcode);
        void inst_9xy0(Opcode opcode);
        void inst_Annn(Opcode opcode);
        void inst_Bnnn(Opcode opcode);
        void inst_Cxkk(Opcode opcode);

        //Display and keyboard related functions
        void inst_Dxyn(Opcode opcode);
        void inst_Ex9E(Opcode opcode);
        void inst_ExA1(Opcode opcode);
        void inst_Fx07(Opcode opcode);
        void inst_Fx0A(Opcode opcode);
        void inst_Fx15(Opcode opcode);
        void inst_Fx18(Opcode opcode);
        void inst_Fx1E(Opcode opcode);
        void inst_Fx29(Opcode opcode);
        void inst_Fx33(Opcode opcode);
        void inst_Fx55(Opcode opcode);
        void inst_Fx65(Opcode opcode);
        void inst_0xxx(Opcode opcode);
        void inst_8xxx(Opcode opcode);
        void inst_Exxx(Opcode opcode);
        void inst_Fxxx(Opcode opcode);
        void execute(Opcode opcode);
};

#include <SDL2/SDL.h>
#include "display_controller.h"

#ifndef CHIP_8
#define CHIP_8

typedef union {
    uint8_t VRAM[4096];
    struct{
        DisplayController display;
        uint8_t font[0x10*0x5];
    };
} Chip8;

extern Chip8 chip8;

#endif

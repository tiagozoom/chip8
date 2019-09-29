#include <SDL2/SDL.h>

#ifndef DISPLAY_H
#define DISPLAY_H
#define DISPLAY_SIZE W * H
#define W 64
#define H 32

extern uint8_t VRAM[4096], DisplayMemory[DISPLAY_SIZE / 8];
extern Uint32 pixels[DISPLAY_SIZE];
#endif

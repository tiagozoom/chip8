#include "display_controller.h"

using namespace std;

DisplayController::DisplayController(){
    window = SDL_CreateWindow("testerino", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W * 4, H * 6, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, W, H);
}

void DisplayController::show(uint8_t* memory, Uint32* pixels){
    /*for(int i = 0; i < (W * H); i++){
           uint16_t index = (7 - i % 8);
           pixels[i] = ((memory[i/8] >> index) & 0x1) ? 0xFF000000 : 0xFFFFFFFF;
    }*/
    SDL_UpdateTexture(texture, nullptr, pixels, W * 4);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void DisplayController::stop(){
    bool quit = false;
    while (!quit){
        SDL_WaitEvent(&event);
        switch(event.type){
            case SDL_QUIT:
                quit = 1;
                break;
        }
    }
}


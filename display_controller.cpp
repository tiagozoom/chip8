#include "display_controller.h"

using namespace std;

void DisplayController::Init(){
    window = SDL_CreateWindow("chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W * 4, H * 6, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, W, H);
}

void DisplayController::show(Uint32* pixels){
    SDL_UpdateTexture(texture, nullptr, pixels, W * 4);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    SDL_Delay(16);
}

bool DisplayController::stop(){
    SDL_Event event;
    bool is_running;
    /*while (!quit){
        SDL_WaitEvent(&event);
        SDL_Delay(1000);
        switch(event.type){
            case SDL_QUIT:
                quit = 1;
                exit = 1;
                break;
            case SDL_KEYUP:
                cout << "I'm here" << endl;
                quit = 1;
                exit = 0;
                break;
        }
    }*/
    return false;
}


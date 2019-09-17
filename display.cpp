#include <iostream>
#include <cstdint>
#include <SDL2/SDL.h>
using namespace std;

#define W 320
#define H 240

int main(int argc, char *argv[]){
    Uint32 pixels[W * H] = {
        0xFFFF0000
    };

    SDL_Rect rect;
    
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        cout << "Error initializing SDL" << endl;
        return 1;
    };

    SDL_Window* window = SDL_CreateWindow("testerino", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_RESIZABLE);
    if(window == nullptr) {
        cout << "Error initializing SDL window" << endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer == nullptr){
        cout << "Error initializing renderer" << endl;
        SDL_Quit();
        return 1;
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, W, H);
    if(texture == nullptr){
        cout << "Error initializing texture" << endl;
        SDL_Quit();
        return 1;
    }

    memset(pixels, 255, W * H * sizeof(Uint32));

    SDL_Event event;
    bool quit = false;
    while (!quit){
        SDL_UpdateTexture(texture, nullptr, pixels, W);
        SDL_WaitEvent(&event);
        switch(event.type){
            case SDL_MOUSEMOTION:
                pixels[0] = 0x0;
                break;
            case SDL_QUIT:
                quit = 1;
                break;
        }

        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
    return 0;
}

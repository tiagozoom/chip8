#include <iostream>
#include <cstdint>
#include <SDL2/SDL.h>
using namespace std;

#define W 64
#define H 32

int main(int argc, char *argv[]){
    Uint32 pixels[W * H] = {
        0xFF000000,
        0x00000000,
        0xFF000000,
        0xFF000000,
        0xFF000000,
        0x00000000,
        0xFF000000,
        0x00000000
    };

    SDL_Rect rect;
    
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        cout << "Error initializing SDL" << endl;
        return 1;
    }};

    SDL_Window* window = SDL_CreateWindow("testerino", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_RESIZABLE);
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
        SDL_UpdateTexture(texture, nullptr, pixels, 64 * 4);
        SDL_WaitEvent(&event);
        int teste[6] = { 0xBA, 0x7C, 0xD6, 0xFE, 0x54, 0xAA};
        int width = 64;
        for(int j = 0; j < 6; j++, width += 64){
            for(int i = 7; i >= 0; i--){
                   pixels[7 - i + width] = ((teste[j] >> i) & 0x1) ? 0xFF000000 : 0xFFFFFFFF ;
            }
        }
	/*pixels[0] = 0xFF000000;
        pixels[1] = 0xFFFFFFFF;
        pixels[2] = 0xFF000000;
        pixels[3] = 0xFF000000;
        pixels[4] = 0xFF000000;
        pixels[5] = 0xFFFFFFFF;
        pixels[6] = 0xFF000000;
        pixels[7] = 0xFFFFFFFF;*/

        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        switch(event.type){
            case SDL_QUIT:
                quit = 1;
                break;
        }
    }
    return 0;
}

#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include "display.h" 

class DisplayController{
    private:
        SDL_Rect rect;
        SDL_Window* window ;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        SDL_Event event;
        uint8_t width;
        uint8_t height;
    public: 
        void Init();
        void show(Uint32* pixels);
        bool stop();
};

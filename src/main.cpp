//
// Created by Ryan Matimu on 8/3/25.
//

# include <cstdlib>
# include <iostream>
# include "bgfx/bgfx.h"
# include <SDL.h>
# include <SDL_metal.h>
# include "bgfx/platform.h"

const uint32_t WNDW_WIDTH = 1600;
const uint32_t WNDW_HEIGHT = 900;

int main()
{

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "SDL + Metal",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WNDW_WIDTH, WNDW_HEIGHT,
        SDL_WINDOW_METAL | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        std::cout << "failed create window" << SDL_GetError()<<std::endl;
        return -1;
    }

    void* metalView = SDL_Metal_CreateView(window);
    void* metalLayer = SDL_Metal_GetLayer(metalView);


    bgfx::PlatformData pdata;
    pdata.nwh = metalLayer;
    pdata.ndt = nullptr;
    bgfx::setPlatformData(pdata);

    bgfx::Init bgfxInit;
    bgfxInit.platformData = pdata;
    bgfxInit.type = bgfx::RendererType::Metal;
    bgfxInit.resolution.width = WNDW_WIDTH;
    bgfxInit.resolution.height = WNDW_HEIGHT;
    bgfxInit.resolution.reset = BGFX_RESET_VSYNC;


    std::cout << "Attempting to initialize bgfx..." << std::endl;
    std::cout << "Renderer type: " << (int)bgfxInit.type << std::endl;
    std::cout << "Resolution: " << bgfxInit.resolution.width << "x" << bgfxInit.resolution.height << std::endl;
    std::cout << "Platform handle (nwh): " << pdata.nwh << std::endl;
    std::cout << "metalLayer: " << metalLayer << std::endl;



    if (!bgfx::init(bgfxInit)) {
        std::cout << "failed bgfx init" << std::endl;
        SDL_Metal_DestroyView(metalView);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }


    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, WNDW_WIDTH, WNDW_HEIGHT);


    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            bgfx::touch(0);
            bgfx::frame();
        }
    }

    SDL_Metal_DestroyView(metalView);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
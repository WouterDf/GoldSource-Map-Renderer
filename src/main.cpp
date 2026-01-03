#include "assetloader.h"
#include "bsp.h"
#include "renderer.h"
#include "windowcontext.h"
#include "SDL3/SDL_events.h"
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>
#include <iostream>
#include <sys/types.h>

int main()
{
    std::cout << "Starting application. \n";
    BSP::BSP map = AssetLoader::readBsp("maps/de_dust2.bsp");

    WindowContext windowContext{};
    Renderer renderer{};

    renderer.Prepare(&map);

    bool done = false;

    bool firstFrame = true;
    unsigned int currentTime, lastTime, deltaTime;
    while( !done )
    {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }

        if( firstFrame )
        {
            firstFrame = false;
            currentTime = SDL_GetTicks();
            lastTime = SDL_GetTicks();
            deltaTime = 0;
            continue;
        }
        currentTime = SDL_GetTicks();
        deltaTime = currentTime - lastTime;


        renderer.DrawFrame(deltaTime);
        SDL_GL_SwapWindow(windowContext.window);
        lastTime = currentTime;

    };

    return 0;
}

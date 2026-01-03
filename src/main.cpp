#include "assetloader.h"
#include "bsp.h"
#include "renderer.h"
#include "windowcontext.h"
#include "SDL3/SDL_events.h"
#include <SDL3/SDL_main.h>
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
    while( !done )
    {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }

        renderer.DrawFrame();
        SDL_GL_SwapWindow(windowContext.window);
    };

    return 0;
}

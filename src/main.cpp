#include "renderer.h"
#include "windowcontext.h"
#include "SDL3/SDL_events.h"
#include <SDL3/SDL_main.h>
#include <iostream>
#include <sys/types.h>

int main()
{
    std::cout << "Starting application. \n";

    WindowContext windowContext{};
    Renderer renderer{};

    renderer.Prepare();

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

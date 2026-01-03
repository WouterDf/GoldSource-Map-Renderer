#include "assetloader.h"
#include "bsp.h"
#include "bsprenderer.h"
#include "camera.h"
#include "testrenderer.h"
#include "windowcontext.h"
#include "SDL3/SDL_events.h"
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>
#include <iostream>
#include <sys/types.h>

void UpdateCamera(Camera* camera, float deltaTime)
{
    float mouseX = 0;
    float mouseY = 0;
    auto camMove = glm::vec3(.0f);
    const bool* keyStates = SDL_GetKeyboardState(nullptr);
    if( keyStates[SDL_SCANCODE_W] )
    {
        camMove -= glm::vec3(0, 0, 1);
    }
    if( keyStates[SDL_SCANCODE_S] )
    {
        camMove += glm::vec3(0, 0, 1);
    }
    if( keyStates[SDL_SCANCODE_A] )
    {
        camMove -= glm::vec3(1, 0, 0);
    }
    if( keyStates[SDL_SCANCODE_D] )
    {
        camMove += glm::vec3(1, 0, 0);
    }
    camera->Move(camMove, deltaTime);
    SDL_GetRelativeMouseState(&mouseX, &mouseY);

    float cameraPitch = 0;
    float cameraYaw = 0;
    if (mouseX != 0)
    {
         cameraYaw += 0.01 * mouseX;
    }
    if (mouseY != 0)
    {
         cameraPitch -= 0.01 * mouseY;
    }
    camera->Rotate(cameraPitch, cameraYaw, deltaTime);
};

int main()
{
    std::cout << "Starting application. \n";
    BSP::BSP map = AssetLoader::readBsp("maps/de_dust2.bsp");

    WindowContext windowContext{};
    auto camera = Camera{glm::vec3(0.0f, 0.0f, 1000.0f), glm::vec3(.0f, .0f, -1.0f)};

    TestRenderer testrenderer{};
    testrenderer.SetCamera(&camera);
    testrenderer.Load();

    BSPRenderer bsprenderer{};
    bsprenderer.SetCamera(&camera);
    bsprenderer.SetMap(&map);
    bsprenderer.Load();

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
        UpdateCamera(&camera, deltaTime);

        //testrenderer.DrawFrame(deltaTime);
        bsprenderer.DrawFrame(deltaTime);

        SDL_GL_SwapWindow(windowContext.window);
        lastTime = currentTime;

    };

    return 0;
}

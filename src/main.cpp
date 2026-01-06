#include "bsp.h"
#include "bsprenderer.h"
#include "camera.h"
#include "testrenderer.h"
#include "windowcontext.h"
#include "SDL3/SDL_events.h"
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>
#include <iostream>
#include <memory>
#include <sys/types.h>

struct FrameTime {
    int currentFrameTime = 0;
    int lastFrameTime = 0;
    int deltaTime = 0;

    void FirstFrameBegin()
    {
        currentFrameTime = SDL_GetTicks();
        lastFrameTime = SDL_GetTicks();
        deltaTime = 0;
    }

    void FrameBegin()
    {
        currentFrameTime = SDL_GetTicks();
        deltaTime = currentFrameTime - lastFrameTime;
    }

    void FrameEnd()
    {
      lastFrameTime = currentFrameTime;
    }
};

void UpdateCamera(Camera* camera, float deltaTime)
{
    float mouseX = 0;
    float mouseY = 0;
    auto camMove = glm::vec3(.0f);
    const bool* keyStates = SDL_GetKeyboardState(nullptr);
    if( keyStates[SDL_SCANCODE_W] )
    {
        camMove += glm::vec3(0, 0, 1);
    }
    if( keyStates[SDL_SCANCODE_S] )
    {
        camMove -= glm::vec3(0, 0, 1);
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
    auto map = std::make_unique<BSP::BSP>("maps/de_dust.bsp");

    WindowContext windowContext{};
    auto camera = Camera{glm::vec3(0.0f, 0.0f, 1500.0f), glm::vec3(.0f, .0f, -1.0f)};

    TestRenderer testrenderer{};
    testrenderer.SetCamera(&camera);
    testrenderer.Load();

    BSPRenderer bsprenderer{};
    bsprenderer.SetCamera(&camera);
    bsprenderer.SetMap(map.get());
    bsprenderer.Load();

    bool done = false;

    FrameTime frameTime{};
    bool firstFrame = true;
    uint frameCount = 0;
    uint lastFpsCountTime = 0;
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
            frameTime.FirstFrameBegin();
            continue;
        }
        frameTime.FrameBegin();
        UpdateCamera(&camera, frameTime.deltaTime);

        //testrenderer.DrawFrame(frameTime.deltaTime);
        bsprenderer.DrawFrame(frameTime.deltaTime);

        // Show FPS
        frameCount++;
        const uint SHOW_FPS_AT_MS = 10000;
        if( frameTime.currentFrameTime - lastFpsCountTime > SHOW_FPS_AT_MS )
        {
             uint fps = frameCount * 1000 / (frameTime.currentFrameTime - lastFpsCountTime);
             std::cout << "Frames per second: " << fps << "\n";
             lastFpsCountTime = frameTime.currentFrameTime;
             frameCount = 0;
        }

        SDL_GL_SwapWindow(windowContext.window);
        frameTime.FrameEnd();
    };

    return 0;
}


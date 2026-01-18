#include "SDL3/SDL_events.h"
#include "bsp.h"
#include "bsprenderer.h"
#include "camera.h"
#include "configuration.h"
#include "windowcontext.h"
#include "worldgeometry.h"
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>
#include <iostream>
#include <memory>
#include <sys/types.h>

struct FrameTime {
  int currentFrameTime = 0;
  int lastFrameTime = 0;
  int deltaTime = 0;

  void FirstFrameBegin() {
    currentFrameTime = SDL_GetTicks();
    lastFrameTime = SDL_GetTicks();
    deltaTime = 0;
  }

  void FrameBegin() {
    currentFrameTime = SDL_GetTicks();
    deltaTime = currentFrameTime - lastFrameTime;
  }

  void FrameEnd() { lastFrameTime = currentFrameTime; }
};

void UpdateCamera(Camera *camera, float deltaTime) {
  float mouseX = 0;
  float mouseY = 0;
  bool boost = false;
  auto camMove = glm::vec3(.0f);
  const bool *keyStates = SDL_GetKeyboardState(nullptr);
  if ( keyStates[SDL_SCANCODE_W] ) {
    camMove += glm::vec3(0, 0, 1);
  }
  if ( keyStates[SDL_SCANCODE_S] ) {
    camMove -= glm::vec3(0, 0, 1);
  }
  if ( keyStates[SDL_SCANCODE_A] ) {
    camMove -= glm::vec3(1, 0, 0);
  }
  if ( keyStates[SDL_SCANCODE_D] ) {
    camMove += glm::vec3(1, 0, 0);
  }
  if( keyStates[SDL_SCANCODE_LSHIFT]
      || keyStates[SDL_SCANCODE_RSHIFT] ) {
      boost = true;
  }

  camera->Move(camMove, deltaTime, boost);
  SDL_GetRelativeMouseState(&mouseX, &mouseY);

  float cameraPitch = 0;
  float cameraYaw = 0;
  if (mouseX != 0) {
    cameraYaw += 0.01 * mouseX;
  }
  if (mouseY != 0) {
    cameraPitch -= 0.01 * mouseY;
  }
  camera->Rotate(cameraPitch, cameraYaw, deltaTime);
};

int main() {
  Configuration configuration{};

  auto map = std::make_unique<BSP::BSP>(configuration.GetMapName());

  WindowContext windowContext{};
  Camera camera{
      configuration.GetCameraStartPosition(),
      glm::vec3(.0F, .0F, -1.0F)
  };
  BSPRenderer bsprenderer{};
  bsprenderer.SetCamera(&camera);
  bsprenderer.SetLightMapsEnabled(configuration.GetEnableLightMaps());

  auto worldGeometry = std::make_unique<WorldGeometry>(&bsprenderer);
  worldGeometry->Load(*map);

  bool done = false;

  FrameTime frameTime{};
  bool firstFrame = true;
  unsigned int frameCount = 0;
  unsigned int lastFpsCountTime = 0;
  while (!done) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        done = true;
      }
    }

    if (firstFrame) {
      firstFrame = false;
      frameTime.FirstFrameBegin();
      continue;
    }
    frameTime.FrameBegin();
    UpdateCamera(&camera, static_cast<float>(frameTime.deltaTime));

    bsprenderer.ClearFrame();
    worldGeometry->DrawFrame();

    // Show FPS
    frameCount++;
    const unsigned int SHOW_FPS_AT_MS = 10000;
    if (frameTime.currentFrameTime - lastFpsCountTime > SHOW_FPS_AT_MS) {
      unsigned int fps =
          frameCount * 1000 / (frameTime.currentFrameTime - lastFpsCountTime);
      std::cout << "Frames per second: " << fps << "\n";
      lastFpsCountTime = frameTime.currentFrameTime;
      frameCount = 0;

      std::cout << "Camera is at position: (" << camera.GetPosition().x
                << ", " << camera.GetPosition().y
                << ", " << camera.GetPosition().z
                << ") \n";
    }

    SDL_GL_SwapWindow(windowContext.window);
    frameTime.FrameEnd();
  };

  return 0;
}

#pragma once

class Camera;

class Renderer {
public:
     virtual void Load() = 0;
     virtual void DrawFrame(float deltaTime) = 0;
     virtual void SetCamera(Camera* camera) = 0;
};

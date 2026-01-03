#pragma once

#include <GL/glew.h>
#include <OpenGL/gltypes.h>
#include <SDL3/SDL_events.h>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include "camera.h"
#include "shader.h"
#include "texture.h"

namespace BSP {
class BSP;
} // namespace BSP

class Renderer {
public:
     void Prepare(BSP::BSP* map);
     void DrawFrame(float deltaTime);

private :
     std::unique_ptr<Shader> shader;
     std::unique_ptr<Texture> texture1;
     std::unique_ptr<Texture> texture2;
     GLuint vao;
     GLuint vbo;
     GLuint ebo;
     GLuint texture;
     int nIndices;
     Camera camera{glm::vec3(0.0f, 0.0f, 1000.0f), glm::vec3(.0f, .0f, -1.0f)};

     void UpdateCamera(float deltaTime);
};

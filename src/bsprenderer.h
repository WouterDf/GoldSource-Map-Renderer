#pragma once

#include <GL/glew.h>
#include <OpenGL/gltypes.h>
#include <SDL3/SDL_events.h>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include "camera.h"
#include "renderer.h"
#include "shader.h"
#include "texture.h"

namespace BSP {
class BSP;
} // namespace BSP


class BSPRenderer : public Renderer {
public:
     void Load() override;
     void DrawFrame(float deltaTime) override;
     void SetCamera(Camera* cam) override;
     void SetMap(BSP::BSP* map);

private :
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Texture> texture1;
    std::unique_ptr<Texture> texture2;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint texture;
    int nIndices;
    Camera* camera;
    BSP::BSP* map;
};

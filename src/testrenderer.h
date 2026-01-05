#pragma once

#include <GL/glew.h>
#include <OpenGL/gltypes.h>
#include <SDL3/SDL_events.h>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include "WADTexture.h"
#include "camera.h"
#include "renderer.h"
#include "shader.h"
#include "pngtexture.h"
#include "wad.h"

namespace BSP {
class BSP;
} // namespace BSP

class TestRenderer: public Renderer {
public:
     void Load() override;
     void DrawFrame(float deltaTime) override;
     void SetCamera(Camera* cam) override;
private :
     std::unique_ptr<Shader> shader;
     std::unique_ptr<PNGTexture> texture1;
     std::unique_ptr<PNGTexture> texture2;
     std::unique_ptr<WADTexture> texture3;
     std::unique_ptr<WAD::WADArchive> wadArchive;
     GLuint vao;
     GLuint vbo;
     GLuint ebo;
     GLuint texture;
     int nIndices;
     Camera* camera;
};

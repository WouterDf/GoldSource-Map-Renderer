#pragma once

#include <GL/glew.h>
#include <OpenGL/gltypes.h>
#include <SDL3/SDL_events.h>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <vector>
#include "WADTexture.h"
#include "bsprenderbatch.h"
#include "camera.h"
#include "renderer.h"
#include "shader.h"
#include "pngtexture.h"
#include "wad.h"

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
    std::unique_ptr<PNGTexture> texture1;
    std::unique_ptr<PNGTexture> texture2;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint texture;
    int nIndices;
    Camera* camera;
    BSP::BSP* map;
    std::unique_ptr<WAD::WAD> wad;
    std::vector<BSPRenderBatch> renderBatches;
    std::vector<WADTexture> textureArray;
};

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
#include "lightmaptexture.h"
#include "shader.h"
#include "pngtexture.h"
#include "wad.h"

class BSPRenderer {
public:
     void DrawFrame(BSPDrawCall drawCall);

     void ClearFrame();

     void SetCamera(Camera* cam);

     std::vector<BSPDrawCall> RegisterDrawCalls(
          std::vector<std::vector<float>> localVertexBuffer,
          std::vector<std::vector<uint32_t>> localIndexBuffer,
          std::vector<std::string> textureName,
          std::vector<LightMapData*> lightMapData);

private :
    void Commit(std::vector<float> vertexBuffer,
                std::vector<uint32_t> indexBuffer,
                std::vector<std::string> textureNames,
                std::vector<LightMapData*> lightMapData);

    std::unique_ptr<Shader> shader;
    std::unique_ptr<WAD::WADArchive> m_wadArchive;
    std::vector<WADTexture> textures;
    std::vector<LightMapTexture> lightMaps;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint texture;
    int nIndices;
    Camera* camera;
};

#include "bsprenderer.h"
#include <GL/glew.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_timer.h>
#include <algorithm>
#include <cstdint>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <memory>
#include <simage.h>
#include <string>
#include <vector>

#include "wadtexture.h"
#include "bsprenderbatch.h"
#include "camera.h"
#include "lightmaptexture.h"
#include "pngtexture.h"
#include "shader.h"
#include "texture.h"


void BSPRenderer::SetCamera(Camera *cam)
{
    camera = cam;
}

std::vector<BSPDrawCall> BSPRenderer::RegisterDrawCalls(
    std::vector<std::vector<float>> localVertexBuffers,
    std::vector<std::vector<uint32_t>> localIndexBuffers,
    std::vector<std::string> textureNames,
    std::vector<const LightMapData*> lightMapData)
{
    constexpr uint8_t ATTRIBUTES_PER_VERTEX = 7;

    std::vector<float> globalVertexBuffer;
    std::vector<uint32_t> globalIndexBuffer;
    std::vector<std::string> globalTextureNames;
     
    std::vector<BSPDrawCall> drawCalls;
    uint32_t numFaces = localVertexBuffers.size();

    for( int faceId = 0; faceId < numFaces ; faceId++)
    {
        uint32_t prevIndexBufferSize = globalIndexBuffer.size();
        uint32_t prevVertexBufferSize = globalVertexBuffer.size();

        for( const auto& vertex: localVertexBuffers[faceId] )
        {
            globalVertexBuffer.push_back(vertex);
        }

        for( const auto& index: localIndexBuffers[faceId] )
        {
            globalIndexBuffer.push_back( index + prevVertexBufferSize / ATTRIBUTES_PER_VERTEX);
        }

        BSPDrawCall drawCall{};
        drawCall.indexOffset = prevIndexBufferSize;
        drawCall.indexLength = globalIndexBuffer.size() - prevIndexBufferSize;

        auto textureName = textureNames[faceId];
        auto textureHit = std::find(globalTextureNames.begin(),
                                    globalTextureNames.end(),
                                    textureName);
        if( textureHit != globalTextureNames.end() )
        {
            drawCall.textureIndex = textureHit - globalTextureNames.begin();
        } else {
            drawCall.textureIndex = globalTextureNames.size();
            globalTextureNames.push_back(textureName);
        }

        drawCall.lightMapIndex = faceId;

        drawCalls.push_back(drawCall);
    }

    Commit(globalVertexBuffer, globalIndexBuffer, globalTextureNames, lightMapData);

    return drawCalls;
}

void BSPRenderer::Commit(std::vector<float> vertexBuffer,
                         std::vector<uint32_t> indexBuffer,
                         std::vector<std::string> textureNames,
                         std::vector<const LightMapData*> lightMapData)
{
    // Shaders
    this->shader = std::make_unique<Shader>(
        "shaders/bsp.vert",
        "shaders/bsp.frag"
    );

    // load textures
    this->m_wadArchive = std::make_unique<WAD::WADArchive>();

    for( const auto& textureName : textureNames )
    {
        WADTexture texture{
            textureName,
            m_wadArchive.get(),
            "texture1",
            shader.get(),
            0
        };
        texture.Load();
        this->textures.push_back(texture); 
    };

    for( const auto& lightMap : lightMapData )
    {
        LightMapTexture texture{
            "texture2",
            shader.get(),
            1,
            lightMap
        };

        if( lightMap->width == 0)
        {
            texture.unused = true;
        } else {
            texture.Load();
        }

        this->lightMaps.push_back(texture);
    }

    std::cout << "Loaded all textures into vram. Amount: " << this->textures.size() << "\n";

    // buffers
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Attrib Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Attrib UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Attrib Lightmap Coordinats
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &ebo);

    // Push buffer data of BSP
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), vertexBuffer.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(uint32_t), indexBuffer.data(), GL_STATIC_DRAW);
}

void BSPRenderer::SetLightMapsEnabled(bool enabled)
{
    m_enableLightMaps = enabled;
}

void BSPRenderer::ClearFrame() {
    glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);  
    
    shader->Use();
    glBindVertexArray(vao);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 1.0f, 10000.0f);
    shader->BindUniform4f("model", glm::value_ptr(model));
    shader->BindUniform4f("view", glm::value_ptr(view));
    shader->BindUniform4f("projection", glm::value_ptr(projection));
}

void BSPRenderer::DrawFrame(BSPDrawCall drawCall) {
    this->textures[drawCall.textureIndex].Use();
    if( m_enableLightMaps )
    {
        this->lightMaps[drawCall.lightMapIndex].Use();
    }

    glDrawElements(GL_TRIANGLES,
                   drawCall.indexLength,
                   GL_UNSIGNED_INT,
                   (void*)(drawCall.indexOffset*sizeof(unsigned int)));

}

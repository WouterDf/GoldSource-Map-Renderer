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

#include "WADTexture.h"
#include "bsp.h"
#include "bsprenderbatch.h"
#include "camera.h"
#include "pngtexture.h"
#include "shader.h"
#include "texture.h"


void BSPRenderer::SetCamera(Camera *cam)
{
     camera = cam;
}

void BSPRenderer::SetMap(BSP::BSP *map) {
     this->map = map;
}

void BSPRenderer::Load() {
this->m_wadArchive = std::make_unique<WAD::WADArchive>(
        std::vector<std::string>{
             "textures/halflife.wad",
             "textures/ajawad.wad",
             "textures/cached.wad",
             "textures/chateau.wad",
             "textures/cs_747.wad",
             "textures/cs_assault.wad",
             "textures/cs_bdog.wad",
             "textures/cs_cbble.wad",
             "textures/cs_dust.wad",
             "textures/cs_havana.WAD",
             "textures/cs_office.wad",
             "textures/cstraining.wad",
             "textures/cstrike.wad",
             "textures/de_airstrip.wad",
             "textures/de_aztec.wad",
             "textures/de_piranesi.wad",
             "textures/de_storm.wad",
             "textures/de_vertigo.wad",
             "textures/decals.wad",
             "textures/fonts.wad",
             //"textures/iga_static.wad",
             "textures/itsitaly.wad",
             "textures/liquids.wad",
             "textures/prodigy.wad",
             "textures/spraypaint.wad",
             "textures/tempdecal.wad",
             "textures/torntextures.wad",
             "textures/tswad.wad",
         });

     const uint8_t ATTRIBUTES_PER_VERTEX = 5;

     std::vector<float> vertexBufferData{};
     std::vector<uint32_t> elementBufferData{};
     
     // Cache of invalid textures
     auto missingTextureCache = std::vector<std::string>{};
     auto validTextureCache = std::vector<std::string>{};

     // Populate the vertex and index buffers for the static level
     // geometry, face per face.
     for( const auto& face : map->faces )
     {
          BSP::Textureinfo textureInfo = map->textureInfos[face.iTextureInfo];
          BSP::MipTex textureMip = map->textures[textureInfo.iMiptex];

          // Add textures
          const auto missingTextureCacheHit = std::find(
                missingTextureCache.begin(),
                missingTextureCache.end(),
                textureMip.szName);
          const auto validTextureCacheHit = std::find(
                validTextureCache.begin(),
                validTextureCache.end(),
                textureMip.szName);

          // Index of this faces texture in textureArray
          uint32_t textureIndex = 0;

          // TODO: This is O(N), should be handled with map for O(1)
          if( missingTextureCacheHit != missingTextureCache.end() )
          {
               // texture was previously detected as invalid;
               continue; // dont draw this face
          }
          else if( validTextureCacheHit != validTextureCache.end() )
          {
               // texture was previously detected as valid
               textureIndex = validTextureCacheHit - validTextureCache.begin();
          }
          else if( this->m_wadArchive->Contains(textureMip.szName) )
          {
               // texture found in WAD-file. It is valid.
               validTextureCache.push_back(textureMip.szName);
               textureIndex = validTextureCache.size() - 1;
          } else {
               // texture not found in WAD. It is invalid
               missingTextureCache.push_back(textureMip.szName);
               continue; // dont draw this face
          }

          // Local vertex buffer
          std::vector<float> faceVertexBuffer{};
          // Local index buffer
          std::vector<uint32_t> faceIndexBuffer{};

          // Store all BSP Vertex indices for each face
          std::vector<uint32_t> faceVertexBSPIndices{};


          for ( int i = 0; i < face.nEdges; i++ )
          {
               int32_t surfedge = map->surfEdges.at(face.iFirstEdge + i);
               uint32_t vertexIndex;

               if (surfedge >= 0)
               {
                    vertexIndex = map->edges[surfedge].iVertex[0];
               } else
               {
                    vertexIndex = map->edges[-surfedge].iVertex[1];
               }
               faceVertexBSPIndices.push_back(vertexIndex);
          }

          // Add vertices to the VBO
          for( const uint32_t& bspVertexIndex : faceVertexBSPIndices )
          {

               BSP::ValveVector3d vPos = map->vertices[bspVertexIndex];
               faceVertexBuffer.push_back( vPos.x );
               faceVertexBuffer.push_back( vPos.z );
               faceVertexBuffer.push_back( -vPos.y );
               auto glmPos = glm::vec3(vPos.x, vPos.y, vPos.z);
               auto glmS = glm::vec3(textureInfo.vS.x, textureInfo.vS.y, textureInfo.vS.z);
               auto glmT = glm::vec3(textureInfo.vT.x, textureInfo.vT.y, textureInfo.vT.z);

               float u = (glm::dot(glmPos, glmS) + textureInfo.fSShift) / textureMip.nWidth;
               float v = (glm::dot(glmPos, glmT) + textureInfo.fTShift) / textureMip.nHeight;

               faceVertexBuffer.push_back(u);
               faceVertexBuffer.push_back(v);
          }

          // Triangulate using triangle fan (v0, v1, v2), (v0, v2, v3), etc.
          // and add incides to local EBO.
          for ( int i = 1; i < face.nEdges - 1; i++ )
          {
               faceIndexBuffer.push_back( 0 );     // First vertex
               faceIndexBuffer.push_back( i );     // Current vertex
               faceIndexBuffer.push_back( i + 1 ); // Next vertex
          }

          uint32_t prevElementBufferSize = elementBufferData.size();
          uint32_t prevVertexBufferSize = vertexBufferData.size();

          // Concat local buffers to global buffers.
          for( const auto& vertex : faceVertexBuffer )
          {
               vertexBufferData.push_back(vertex);
          }

          for( const auto& index : faceIndexBuffer )
          {
               elementBufferData.push_back( index + prevVertexBufferSize / ATTRIBUTES_PER_VERTEX);
          }

          

          // Create RenderBatch for face
          BSPRenderBatch renderBatch{};
          renderBatch.indexOffset = prevElementBufferSize;
          renderBatch.indexLength = elementBufferData.size() - prevElementBufferSize;
          renderBatch.textureIndex = textureIndex;
          this->renderBatches.push_back(renderBatch);
     };

     std::cout << "Number of valid textures: " << validTextureCache.size() << "\n";
     std::cout << "Number of missing textures: " << missingTextureCache.size() << "\n";
     for( const auto& textureName : missingTextureCache )
     {
          std::cout << textureName << "\n";
     }

    // Shaders
    this->shader = std::make_unique<Shader>(
         "shaders/bsp.vert",
         "shaders/bsp.frag"
         );

        // Test texture
    texture1 = std::make_unique<WADTexture>(
         "FIFTIES_WALL6B",
         m_wadArchive.get(),
         "texture1",
         shader.get(),
         0
         );

    texture1->Load();

    // load textures
    for( const auto& textureName : validTextureCache )
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

    std::cout << "Loaded all textures into vram. Amount: " << this->textures.size() << "\n";

    // buffers
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Attrib Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Attrib UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ebo);

    // Push buffer data of BSP
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferData.size() * sizeof(float), vertexBufferData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementBufferData.size() * sizeof(uint32_t), elementBufferData.data(), GL_STATIC_DRAW);
}

void BSPRenderer::DrawFrame(float deltaTime) {
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

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for( const auto& renderBatch : this->renderBatches )
    {
        this->textures[renderBatch.textureIndex].Use();

        glDrawElements(GL_TRIANGLES,
                       renderBatch.indexLength,
                       GL_UNSIGNED_INT,
                       (void*)(renderBatch.indexOffset*sizeof(unsigned int)));
    }
}

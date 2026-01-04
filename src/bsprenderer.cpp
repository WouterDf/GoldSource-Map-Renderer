#include "bsprenderer.h"
#include <GL/glew.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_timer.h>
#include <cstdint>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <memory>
#include <simage.h>

#include "bsp.h"
#include "bsprenderbatch.h"
#include "camera.h"
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
     const uint8_t ATTRIBUTES_PER_VERTEX = 3;

     std::vector<float> vertexBufferData{};
     std::vector<uint32_t> elementBufferData{};
     
     // Build a local vertex- and index-buffer for each face
     // Generate a RenderBatch for each face
     for( const auto& face : map->faces )
     {
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
               faceVertexBuffer.push_back( map->vertices[bspVertexIndex].x );
               faceVertexBuffer.push_back( map->vertices[bspVertexIndex].z );
               faceVertexBuffer.push_back( map->vertices[bspVertexIndex].y );
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
          this->renderBatches.push_back(renderBatch);
     };

     nIndices = elementBufferData.size();

    // Shaders
    this->shader = std::make_unique<Shader>(
         "shaders/bsp.vert",
         "shaders/bsp.frag"
         );

    // buffers
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Attrib Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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
    
    // Uniform
    shader->Use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 1.0f, 10000.0f);
    shader->BindUniform4f("model", glm::value_ptr(model));
    shader->BindUniform4f("view", glm::value_ptr(view));
    shader->BindUniform4f("projection", glm::value_ptr(projection));

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, (void*)(0*sizeof(unsigned int)));
}

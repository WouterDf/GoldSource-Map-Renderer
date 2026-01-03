#include "bsprenderer.h"
#include <GL/glew.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_timer.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <memory>
#include <simage.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "bsprenderer.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "bsp.h"


void BSPRenderer::SetCamera(Camera *cam)
{
     camera = cam;
}

void BSPRenderer::SetMap(BSP::BSP *map) {
     this->map = map;
}

void BSPRenderer::Load() {
     std::vector<float> vertices{};
     std::vector<uint32_t> mapindices{};

     for( const auto& vert : map->vertices )
     {
          vertices.push_back(vert.x);
          vertices.push_back(vert.z);
          vertices.push_back(vert.y);
     }

     for( const auto& face : map->faces )
     {
          // First, build the vertex list for this face
          std::vector<uint32_t> faceVertices{};
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
               faceVertices.push_back(vertexIndex);
          }

          // Now triangulate using triangle fan (v0, v1, v2), (v0, v2, v3), etc.
          for ( int i = 1; i < face.nEdges - 1; i++ )
          {
               mapindices.push_back(faceVertices[0]);     // First vertex
               mapindices.push_back(faceVertices[i]);     // Current vertex
               mapindices.push_back(faceVertices[i + 1]); // Next vertex
          }
     };
     nIndices = mapindices.size();

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
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mapindices.size() * sizeof(uint32_t), mapindices.data(), GL_STATIC_DRAW);
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

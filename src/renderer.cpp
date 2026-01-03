#include <GL/glew.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_timer.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <memory>
#include <simage.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderer.h"
#include "shader.h"
#include "texture.h"
#include "bsp.h"

void Renderer::Prepare(BSP::BSP* level) {
     std::vector<float> vertices{};
     std::vector<uint32_t> mapindices{};

     for( const auto& vert : level->vertices )
     {
          vertices.push_back(vert.x);
          vertices.push_back(vert.z);
          vertices.push_back(vert.y);
     }

     for( const auto& face : level->faces )
     {
          // First, build the vertex list for this face
          std::vector<uint32_t> faceVertices{};
          for ( int i = 0; i < face.nEdges; i++ )
          {
               int32_t surfedge = level->surfEdges.at(face.iFirstEdge + i);
               uint32_t vertexIndex;

               if (surfedge >= 0)
               {
                    vertexIndex = level->edges[surfedge].iVertex[0];
               } else
               {
                    vertexIndex = level->edges[-surfedge].iVertex[1];
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

    float quadVertices[] = {
         // position         // texture uv
         -500.0f, -500.0f, 0.0f,  0.0f, 0.0f, // bottom left
          500.5f, -500.5f, 0.0f, 1.0f, 0.0f, // bottom right
          500.5f,  500.5f, 0.0f, 1.0f, 1.0f, // top right
         -500.5f,  500.5f, 0.0f, 0.0f, 1.0f, // top left

         -500.5f, -500.5f, 0.0f,  0.0f, 0.0f, // 4 rear left
          500.5f, -500.5f, 0.0f,  1.0f, 0.0f, // 5 rear right
         -500.5f, -0.7f, -0.5f,  0.0f, 1.0f, // 6 front left
          500.5f, -0.7f, -0.5f,  1.0f, 1.0f, // 7 front right
     };

    GLuint indices[] = {
         0, 3, 2,
         2, 1, 0,
         7, 5, 4,
         7, 6, 4
    };

    // Shaders
    this->shader = std::make_unique<Shader>(
         "shaders/shader.vert",
         "shaders/shader.frag"
         );

    // buffers
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // Position (only 3 floats per vertex for BSP data)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
        // Color
    //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(3*sizeof(float)));
    //glEnableVertexAttribArray(1);
        // UV Coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ebo);

    // Push buffer data of BSP
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, mapindices.size() * sizeof(uint32_t), mapindices.data(), GL_STATIC_DRAW);
    
    // Push buffer data of quad
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof( quadVertices ), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW);


    // Textures
    this->texture1 = std::make_unique<Texture>(
         "textures/wall.png",
         "texture1",
         shader.get(),
         0);
    this->texture1->Load();
    this->texture2 = std::make_unique<Texture>(
         "textures/wall2.png",
         "texture2",
         shader.get(),
         1);
    this->texture2->Load();

}

void Renderer::UpdateCamera(float deltaTime)
{
    float mouseX = 0;
    float mouseY = 0;
    auto camMove = glm::vec3(.0f);
    const bool* keyStates = SDL_GetKeyboardState(nullptr);
    if( keyStates[SDL_SCANCODE_W] )
    {
        camMove -= glm::vec3(0, 0, 1);
    }
    if( keyStates[SDL_SCANCODE_S] )
    {
        camMove += glm::vec3(0, 0, 1);
    }
    if( keyStates[SDL_SCANCODE_A] )
    {
        camMove -= glm::vec3(1, 0, 0);
    }
    if( keyStates[SDL_SCANCODE_D] )
    {
        camMove += glm::vec3(1, 0, 0);
    }
    camera.Move(camMove, deltaTime);
    SDL_GetRelativeMouseState(&mouseX, &mouseY);

    float cameraPitch = 0;
    float cameraYaw = 0;
    if (mouseX != 0)
    {
         cameraYaw += 0.01 * mouseX;
    }
    if (mouseY != 0)
    {
         cameraPitch -= 0.01 * mouseY;
    }
    camera.Rotate(cameraPitch, cameraYaw, deltaTime);
};

void Renderer::DrawFrame(float deltaTime) {
    UpdateCamera(deltaTime);

    glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);  
    
    shader->Use();
    texture1->Use();
    texture2->Use();
    glBindVertexArray(vao);
    
    // Uniform
    shader->Use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 1.0f, 10000.0f);
    shader->BindUniform4f("model", glm::value_ptr(model));
    shader->BindUniform4f("view", glm::value_ptr(view));
    shader->BindUniform4f("projection", glm::value_ptr(projection));

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, (void*)(0*sizeof(unsigned int)));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0*sizeof(unsigned int)));
}

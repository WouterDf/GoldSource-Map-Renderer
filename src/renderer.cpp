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

#include "renderer.h"
#include "shader.h"
#include "texture.h"

void Renderer::Prepare() {
    float quadVertices[] = {
         // position         // color                // texture uv
         -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
          0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom right
          0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, // top right
         -0.5f,  0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, // top left

         -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 4 rear left
          0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, // 5 rear right
         -0.5f, -0.7f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // 6 front left
          0.5f, -0.7f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, // 7 front right
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
        // Color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
        // UV Coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(7*sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &ebo);

    // Push buffer data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

float dt = 0.0f;

glm::vec3 cameraPosition = glm::vec3(0, 0, 3.0f);
glm::vec3 cameraForward ;
glm::vec3 cameraUp = glm::vec3(0, 1.0f, 0);
float cameraSpeed = 0.005f;
float cameraSensitivity = 1.0f;
float cameraYaw = -90.0;
float cameraPitch = 0.0f;

float mouseX, mouseY;
unsigned int currentTime = 0, lastTime, deltaTime;
void Renderer::DrawFrame() {
     if (currentTime == 0)
     {
          currentTime = SDL_GetTicks();
          lastTime = SDL_GetTicks();
          deltaTime = 0;
          return;
     }
     currentTime = SDL_GetTicks();
     deltaTime = currentTime - lastTime;
     lastTime = currentTime;

    const bool* keyStates = SDL_GetKeyboardState(nullptr);
    if( keyStates[SDL_SCANCODE_W] )
    {
        cameraPosition -= glm::vec3(0, 0, cameraSpeed * deltaTime);
    }
    if( keyStates[SDL_SCANCODE_S] )
    {
        cameraPosition += glm::vec3(0, 0, cameraSpeed * deltaTime);
    }
    if( keyStates[SDL_SCANCODE_A] )
    {
        cameraPosition -= glm::vec3(cameraSpeed * deltaTime, 0, 0);
    }
    if( keyStates[SDL_SCANCODE_D] )
    {
        cameraPosition += glm::vec3(cameraSpeed * deltaTime, 0, 0);
    }
    SDL_GetRelativeMouseState(&mouseX, &mouseY);
    if (mouseX != 0)
    {
         cameraYaw += 0.01 * mouseX * cameraSensitivity * deltaTime;
    }
    if (mouseY != 0)
    {
         cameraPitch += 0.01 * mouseY * cameraSensitivity * deltaTime;
    }
    cameraForward.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    cameraForward.y = sin(glm::radians(cameraPitch));
    cameraForward.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    cameraForward = glm::normalize(cameraForward);

    dt += 0.4f;
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
    model = glm::rotate(model, glm::radians(dt), glm::vec3(0.0, 1.0, 0.0f));
    model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0, 1.0, 0.0f));
    glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraForward, cameraUp);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
    shader->BindUniform4f("model", glm::value_ptr(model));
    shader->BindUniform4f("view", glm::value_ptr(view));
    shader->BindUniform4f("projection", glm::value_ptr(projection));

    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, (void*)(0*sizeof(unsigned int)));
}

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

#include "testrenderer.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"

void TestRenderer::SetCamera(Camera *cam)
{
     camera = cam;
}    

void TestRenderer::Load() {
    float quadVertices[] = {
         // position              // texture uv
         -500.0f, -500.0f, 0.0f,  0.0f, 0.0f, // bottom left
          500.5f, -500.5f, 0.0f,  1.0f, 0.0f, // bottom right
          500.5f,  500.5f, 0.0f,  1.0f, 1.0f, // top right
         -500.5f,  500.5f, 0.0f,  0.0f, 1.0f, // top left

         -500.5f, -500.5f, 0.0f,  0.0f, 0.0f, // 4 rear left
          500.5f, -500.5f, 0.0f,  1.0f, 0.0f, // 5 rear right
         -500.5f, -0.7f, -0.5f,   0.0f, 1.0f, // 6 front left
          500.5f, -0.7f, -0.5f,   1.0f, 1.0f, // 7 front right
     };

    GLuint indices[] = {
         0, 3, 2,
         2, 1, 0,
    };

    // Shaders
    this->shader = std::make_unique<Shader>(
         "shaders/testrenderer.vert",
         "shaders/testrenderer.frag"
         );

    // buffers
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Attrib position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Attrib UV Coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ebo);

    // Push buffer data of quad
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof( quadVertices ), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW);


    // Textures
    this->texture1 = std::make_unique<PNGTexture>(
         "textures/wall.png",
         "texture1",
         shader.get(),
         0);
    this->texture1->Load();
    this->texture2 = std::make_unique<PNGTexture>(
         "textures/wall2.png",
         "texture2",
         shader.get(),
         1);
    this->texture2->Load();
}

float rot = 0;
void TestRenderer::DrawFrame(float deltaTime) {
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
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 1.0f, 10000.0f);

    rot += 0.05 * deltaTime;
    model = glm::rotate(model, glm::radians(rot), glm::vec3(.0f, 1.0f, .0f));

    shader->BindUniform4f("model", glm::value_ptr(model));
    shader->BindUniform4f("view", glm::value_ptr(view));
    shader->BindUniform4f("projection", glm::value_ptr(projection));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0*sizeof(unsigned int)));
}

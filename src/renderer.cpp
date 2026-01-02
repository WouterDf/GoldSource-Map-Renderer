#include <GL/glew.h>
#include <memory>
#include <simage.h>

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

void Renderer::DrawFrame() {
    glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shader->Use();
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
}

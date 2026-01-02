#pragma once

#include <GL/glew.h>
#include <OpenGL/gltypes.h>
#include <SDL3/SDL_events.h>
#include <memory>
#include "shader.h"
#include "texture.h"


class Renderer {
private :
     std::unique_ptr<Shader> shader;
     std::unique_ptr<Texture> texture1;
     std::unique_ptr<Texture> texture2;
     GLuint vao;
     GLuint vbo;
     GLuint ebo;
     GLuint texture;
public:
     void Prepare();
     void DrawFrame();
};

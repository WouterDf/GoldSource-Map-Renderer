#pragma once

#include <GL/glew.h>
#include <OpenGL/gltypes.h>
#include <memory>
#include "shader.h"

class Renderer {
private :
     std::unique_ptr<Shader> shader;
     GLuint vao;
     GLuint vbo;
     GLuint ebo;

public:
     explicit Renderer() {};
     void prepare();
     void drawFrame();
};

#pragma once

#include <string>
#include <GL/glew.h>

class Shader;

class Texture {
public:
     Texture(std::string uniformName, Shader* shader, unsigned int texturenum);
     virtual void Load() = 0;
     virtual void Use() = 0;
protected:
     Shader* shader;
     std::string uniformName;
     GLuint textureId;
     unsigned int texturenum = 0;
};

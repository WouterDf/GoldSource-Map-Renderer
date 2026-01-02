#pragma once

#include <string>
#include <GL/glew.h>

class Shader;

class Texture {
public:
     Texture(std::string relativePath, std::string uniformName, Shader* shader, unsigned int texturenum);
     void Load();
     void Use();
     
private:
     Shader* shader;
     std::string relativePath;
     std::string uniformName;
     GLuint textureId;
     unsigned int texturenum = 0;
};

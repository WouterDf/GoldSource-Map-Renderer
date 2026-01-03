#pragma once

#include <string>
#include <GL/glew.h>

#include "texture.h"

class Shader;

class PNGTexture : public Texture {
public:
     PNGTexture(std::string relativePath, std::string uniformName, Shader* shader, unsigned int texturenum);
     void Load() override;
     void Use() override;
     
private:
     std::string relativePath;
};

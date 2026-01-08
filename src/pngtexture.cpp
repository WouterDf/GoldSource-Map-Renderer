#include "pngtexture.h"
#include "texture.h"
#include "assetloader.h"
#include "shader.h"
#include <iostream>

PNGTexture ::PNGTexture(std::string relativePath, std::string uniformName,
                  Shader *shader, unsigned int texturenum)
     : Texture(uniformName, shader, texturenum), relativePath(relativePath) {}

void PNGTexture::Load() {
    glGenTextures(1, &this->textureId);
    glActiveTexture(GL_TEXTURE0 + texturenum);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    shader->Use();
    shader->BindUniform(uniformName, texturenum);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nChannels;
    unsigned char* data;
    data = AssetLoader::ReadImage(relativePath, &width, &height, &nChannels);
    if( !data )
    {
         std::cout << "Could not read texture. \n";
    } else {
         std::cout << "Texture loaded\n";
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
         glGenerateMipmap(GL_TEXTURE_2D);
    }
  
};

void PNGTexture::Use() {
     glActiveTexture(GL_TEXTURE0 + texturenum);
     glBindTexture(GL_TEXTURE_2D, this->textureId);
};
 

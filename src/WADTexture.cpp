#include "WADTexture.h"
#include "shader.h"
#include "wad.h"
#include <cstdint>
#include <iostream>

WADTexture::WADTexture(std::string textureName, WAD::WAD *wadArchive,
                       std::string uniformname, Shader *shader,
                       unsigned int texturenum)
    : Texture(uniformname, shader, texturenum), m_textureName(textureName),
      m_wadArchive( wadArchive ) {}

void WADTexture::Load()
{
    glGenTextures(1, &this->textureId);
    glActiveTexture(GL_TEXTURE0 + texturenum);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    shader->Use();
    shader->BindUniform(uniformName, texturenum);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::vector<uint8_t> rgba;
    if( !m_wadArchive->Contains(m_textureName) )
    {
         std::cout << "Could not find texture in WAD Archive" << "\n";
         return;
    }

    unsigned int width = 0;
    unsigned int height = 0;
    rgba = m_wadArchive->LoadTexture(m_textureName, &width, &height);

    if( !rgba.data() )
    {
         std::cout << "Could not read texture. \n";
    } else {
         std::cout << "Texture loaded\n";
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
         glGenerateMipmap(GL_TEXTURE_2D);
    }
}

void WADTexture::Use()
{
    glActiveTexture(GL_TEXTURE0 + texturenum);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
}


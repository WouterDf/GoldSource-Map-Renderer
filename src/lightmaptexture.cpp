#include "lightmaptexture.h"
#include "shader.h"
#include "texture.h"
#include <cstdint>
#include <stdexcept>

LightMapTexture::LightMapTexture(std::string uniformName, Shader* shader,
                                 unsigned int texturenum, const LightMapData* data)
     : Texture(uniformName, shader, texturenum), p_data( data ){};

void LightMapTexture::Load()
{
    if( p_data->rgb.empty() )
    {
         throw std::runtime_error("Could not not lightmap texture");
    }

    glGenTextures(1, &this->textureId);
    glActiveTexture(GL_TEXTURE0 + texturenum);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    shader->Use();
    shader->BindUniform(uniformName, texturenum);
    /**
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    **/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(
         GL_TEXTURE_2D,
         0,
         GL_RGB,
         p_data->width,
         p_data->height,
         0,
         GL_RGB,
         GL_UNSIGNED_BYTE,
         p_data->rgb.data());

    glGenerateMipmap(GL_TEXTURE_2D);
};


void LightMapTexture::Use()
{
     if (unused) return;
    glActiveTexture(GL_TEXTURE0 + texturenum);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
};

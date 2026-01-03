#include "texture.h"
#include "shader.h"
#include <string>

Texture::Texture(std::string uniformName, Shader *shader, unsigned int texturenum)
     : uniformName( uniformName ),
       shader( shader ),
       texturenum( texturenum){}


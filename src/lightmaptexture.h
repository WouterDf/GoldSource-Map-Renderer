#pragma once

#include "texture.h"
#include <cstdint>
#include <glm/fwd.hpp>
#include <vector>

struct LightMapData {
     uint32_t width;
     uint32_t height;
     std::vector<uint8_t> rgb;
};

class LightMapTexture : public Texture {
public:
     LightMapTexture(std::string uniformName, Shader* shader,
                     unsigned int texturenum, LightMapData data);

     void Load() override;

     void Use() override;

     bool unused = false;

private:
     LightMapData m_data;
};

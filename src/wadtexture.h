#pragma once

#include "texture.h"
#include "wad.h"
#include <string>

/**
 * Represents a Texture that can be loaded from a GoldSource WAD-file.
 */
class WADTexture : public Texture {
public:
     WADTexture(std::string textureName,
                WAD::WADArchive* wadArchive,
                std::string uniformname,
                Shader* shader,
                unsigned int texturenum);

     void Load() override;

     void Use() override;

private:
     // Name of texture in WAD archive.
     std::string m_textureName;

     // The original archive.
     WAD::WADArchive* m_wadArchive;
};

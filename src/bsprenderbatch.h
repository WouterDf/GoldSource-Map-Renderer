#pragma once

#include <cstdint>
class WADTexture;

/**
 * Represents one draw call for the BSP Renderer
 * (one surface with a material).
 */
struct BSPDrawCall {
public:
     // offset of indices in renderers EBO
     uint32_t indexOffset;

     // length of indices in renderers EBO
     uint32_t indexLength;

     // index of texture in renderers textureArray
     uint32_t textureIndex;
};


#pragma once

#include "lightmaptexture.h"
#include <cstdint>
#include <filesystem>
#include <glm/ext/vector_float3.hpp>
#include <vector>
#include <glm/vec2.hpp>

namespace BSP {

     // BSP Internals
     constexpr int MAXTEXTURENAME = 16;
     constexpr int MIPLEVELS = 4;

     const uint8_t LUMP_ENTITIES     = 0;
     const uint8_t LUMP_PLANES       = 1;
     const uint8_t LUMP_TEXTURES     = 2;
     const uint8_t LUMP_VERTICES     = 3;
     const uint8_t LUMP_VISIBILITY   = 4;
     const uint8_t LUMP_NODES        = 5;
     const uint8_t LUMP_TEXINFO      = 6;
     const uint8_t LUMP_FACES        = 7;
     const uint8_t LUMP_LIGHTING     = 8;
     const uint8_t LUMP_CLIPNODES    = 9;
     const uint8_t LUMP_LEAVES       = 10;
     const uint8_t LUMP_MARKSURFACES = 11;
     const uint8_t LUMP_EDGES        = 12;
     const uint8_t LUMP_SURFEDGES    = 13;
     const uint8_t LUMP_MODELS       = 14;

     struct ValveVector3d {
          float x, y, z;
     };

     struct Lump {
          uint32_t nOffset;
          uint32_t nLength;
     };

     struct Header {
          uint32_t version;
          Lump lump[15];
     };

     struct TextureHeader {
          uint32_t nMipTextures;
     };

     struct MipTex {
          char szName[MAXTEXTURENAME];
          uint32_t nWidth, nHeight;
          uint32_t nOffsets[MIPLEVELS];
     };

     struct Textureinfo {
          ValveVector3d vS;
          float fSShift;
          ValveVector3d vT;
          float fTShift;
          uint32_t iMiptex; // Index into textures array
          uint32_t nFlags;  // Texture flags (0x1 for disabling lightmaps and subdiv surf [sky])
     };

     struct Face {
          uint16_t iPlane;
          uint16_t nPlaneSide;
          uint32_t iFirstEdge; // index of the first surfedge
          uint16_t nEdges;
          uint16_t iTextureInfo;
          uint8_t nStyles[4];
          uint32_t nLightmapOffset;
     };

     struct Edge {
          uint16_t iVertex[2];
     };

     struct LightMapDimensions {
          float minU;
          float maxU;
          float minV;
          float maxV;
          uint32_t width;
          uint32_t height;
     };

     // End of BSP Internals

     class BSP {
     public:
          explicit BSP(std::filesystem::path relativePath);
          std::vector<Face> GetFaces() const { return faces; };
          std::vector<Textureinfo> GetTextureinfos() const { return texInfo; };
          std::vector<MipTex> GetTextures() const { return textures; };
          std::vector<Edge> GetEdges() const { return edges; };

          // Get all vertices for face
          std::vector<glm::vec3> GetVertices(const Face& face) const;

          glm::vec2 GetTextureCoords(glm::vec3 vertexPosition, Face face) const;

          glm::vec2 GetLightmapCoords(glm::vec3 vertexPosition, Face face) const;

          glm::vec2 GetNonNormalizedTextureCoords(glm::vec3 vertexPosition, Face face) const;

          std::vector<LightMapData>& GetLightMaps() { return m_lightMapData; };

     private:
          Header header;
          char* entities;
          TextureHeader textureHeader;
          std::vector<uint32_t> textureOffsets;
          std::vector<Textureinfo> texInfo;
          std::vector<MipTex> textures;
          std::vector<ValveVector3d> vertices;
          std::vector<Face> faces;
          std::vector<Edge> edges;
          std::vector<int32_t> surfEdges;
          std::vector<LightMapData> m_lightMapData;

          LightMapDimensions GetLightmapDimensions(Face face) const;
     };

} // namespace BSP

// end of bsp.h


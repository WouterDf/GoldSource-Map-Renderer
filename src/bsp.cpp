#include <cstdint>
#include <fstream>
#include <iostream>

#include "bsp.h"
#include "wad.h"
#include "assetloader.h"

namespace BSP {
     BSP::BSP(std::filesystem::path relativePath)
    {
          const bool LOG_LUMP_OFFSETS = false;
          const bool LOG_TEXTURES = false;
          const bool LOG_VERTICES = false;
          const bool LOG_FACES = false;
          const bool LOG_EDGES = false;
          const bool LOG_SURFEDGES = false;
          const bool LOG_TEXINFO = false;

          std::ifstream file = AssetLoader::ReadFile(relativePath);

          file.read(reinterpret_cast<char*>(&this->header), sizeof(this->header));
          std::cout << "Version is " << this->header.version << "\n";
          if( LOG_LUMP_OFFSETS )
          {
            for (int i = 0; i < 15; i++)
            {
                    std::cout << "lump " << i << " has length " <<  this->header.lump[i].nLength << "\n";
                    std::cout << " and offset " <<  this->header.lump[i].nOffset << "\n";
            }
          }

          // Entities
          this->entities = new char[this->header.lump[LUMP_ENTITIES].nLength];
          file.seekg(this->header.lump[LUMP_ENTITIES].nOffset);
          file.read(this->entities, this->header.lump[LUMP_ENTITIES].nLength * sizeof(char));

          // Textures
          Lump textureLumpHeader = this->header.lump[LUMP_TEXTURES];
          file.seekg(textureLumpHeader.nOffset);
          file.read(reinterpret_cast<char*>(&this->textureHeader), sizeof(this->textureHeader));

          this->textureOffsets = std::vector<uint32_t>(this->textureHeader.nMipTextures);
          file.read(reinterpret_cast<char*>(this->textureOffsets.data()), sizeof(uint32_t) * this->textureHeader.nMipTextures);

          this->textures = std::vector<MipTex>();
          this->textures.reserve(this->textureHeader.nMipTextures);
          for( const uint32_t& offset : this->textureOffsets )
          {
                file.seekg(textureLumpHeader.nOffset + offset);
                MipTex mipTex;
                file.read(reinterpret_cast<char*>(&mipTex), sizeof(mipTex));
                WAD::NormalizeTextureName(mipTex.szName);
                this->textures.emplace_back(mipTex);
          }

          if( LOG_TEXTURES )
          {
            std::cout << "Amount of textures in map: " << this->textureHeader.nMipTextures << "\n";
            for ( const MipTex& mipTex : this->textures )
            {
                std::cout << "read miptex name: " << mipTex.szName << "\n";
                std::cout << "read miptex width: " << mipTex.nWidth << "\n";
                std::cout << "read miptex height: " << mipTex.nHeight << "\n";
                std::cout << "read miptex offsets: " << mipTex.nOffsets[0] << "\n";
                std::cout << "read miptex offsets: " << mipTex.nOffsets[1] << "\n";
                std::cout << "read miptex offsets: " << mipTex.nOffsets[2] << "\n";
                std::cout << "read miptex offsets: " << mipTex.nOffsets[3] << "\n";
            }
          }

          // Texture infos
          Lump textureInfoLump = this->header.lump[LUMP_TEXINFO];
          int numTextureInfos = textureInfoLump.nLength / sizeof( Textureinfo );
          file.seekg( textureInfoLump.nOffset );
          this->textureInfos = std::vector<Textureinfo>{};
          this->textureInfos.reserve( numTextureInfos );
          for( int i = 0; i < numTextureInfos; i++ )
          {
               Textureinfo texinfo{};
               file.read(reinterpret_cast<char*>(&texinfo), sizeof(texinfo));
               this->textureInfos.push_back(texinfo);
          }

          if( LOG_TEXINFO )
          {
               std::cout << "Number of texture-info-structs: " << numTextureInfos << "\n";

               for( const auto& texinfo : this->textureInfos )
               {
                    std::cout << "Texinfo vS: " << texinfo.vS.x << " " << texinfo.vS.y << " " << texinfo.vS.z << "\n";
                    std::cout << "Texinfo vT: " << texinfo.vT.x << " " << texinfo.vT.y << " " << texinfo.vT.z << "\n";
                    std::cout << "Texinfo SShift: " << texinfo.fSShift << "\n";
                    std::cout << "Texinfo TShift: " << texinfo.fTShift << "\n";
                    std::cout << "Texinfo miptex index: " << texinfo.iMiptex << "\n";
               }
          }

          // Vertices
          Lump verticesLump = this->header.lump[LUMP_VERTICES];
          int numVertices = verticesLump.nLength / sizeof( ValveVector3d );

          this->vertices = std::vector<ValveVector3d>();

          file.seekg(verticesLump.nOffset);
          for (int i = 0; i < numVertices; i++ )
          {
               ValveVector3d vector{};
               file.read(reinterpret_cast<char*>(&vector), sizeof(ValveVector3d));
               this->vertices.push_back(vector);
          }

          if( LOG_VERTICES )
          {
               std::cout << "Number of vertices in map: " << numVertices << "\n";
               if (numVertices > 0) {
                    std::cout << "First vertex: " << this->vertices[0].x << ", " << this->vertices[0].y << ", " << this->vertices[0].z << "\n";
               }
          }

          // Faces
          Lump faceLump = this->header.lump[LUMP_FACES];
          int numFaces = faceLump.nLength / sizeof( Face );
          this->faces = std::vector<Face>();

          file.seekg(faceLump.nOffset);
          for (int i = 0; i < numFaces; i++ )
          {
               Face face{};
               file.read(reinterpret_cast<char*>(&face), sizeof(Face));
               this->faces.push_back(face);
          }


          if( LOG_FACES )
          {
               std::cout << "Number of faces in map: " << numFaces << "\n";
               if (numFaces > 0) {
                    std::cout << "First face has " << this->faces[0].nEdges << " edges, iFirstEdge=" << this->faces[0].iFirstEdge << "\n";
               }
          }

          // EDGES
          Lump edgesLump = this->header.lump[LUMP_EDGES];
          int numEdges = edgesLump.nLength / sizeof( Edge );
          this->edges = std::vector<Edge>();

          file.seekg(edgesLump.nOffset);
          for (int i = 0; i < numEdges; i++ )
          {
               Edge edge{};
               file.read(reinterpret_cast<char*>(&edge), sizeof(Edge));
               this->edges.push_back(edge);
          }


          if( LOG_EDGES )
          {
               std::cout << "Number of edges in map: " << numEdges << "\n";
               for( const Edge& edge : this->edges )
               {
                    std::cout << "Read edge with vertices " << edge.iVertex[0] << " and " << edge.iVertex[1] << "\n";
               }
          }

          // SURFEDGES
          Lump surfEdgesLump = this->header.lump[LUMP_SURFEDGES];
          int numSurfEdges = surfEdgesLump.nLength / sizeof( int32_t );
          this->surfEdges = std::vector<int32_t>();

          file.seekg(surfEdgesLump.nOffset);
          for (int i = 0; i < numSurfEdges; i++ )
          {
               int32_t surfedge;
               file.read(reinterpret_cast<char*>(&surfedge), sizeof(surfedge));
               this->surfEdges.push_back(surfedge);
          }


          if( LOG_SURFEDGES )
          {
               std::cout << "Number of surfedges in map: " << numSurfEdges << "\n";
               for( const int32_t& edge : this->surfEdges )
               {
                    std::cout << "Read surfedge " << edge << "\n";
               }
          }

          std::cout << "Size of map: " << sizeof(this) << "\n";
    }
} // namespace BSP

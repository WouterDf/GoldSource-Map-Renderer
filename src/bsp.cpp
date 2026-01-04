#include <fstream>
#include <iostream>

#include "bsp.h"

namespace BSP {
     BSP ParseBSPFile(std::ifstream file)
    {
          const bool LOG_LUMP_OFFSETS = false;
          const bool LOG_TEXTURES = false;
          const bool LOG_VERTICES = false;
          const bool LOG_FACES = false;
          const bool LOG_EDGES = false;
          const bool LOG_SURFEDGES = false;
          const bool LOG_TEXINFO = false;

          BSP map{};
          file.read(reinterpret_cast<char*>(&map.header), sizeof(map.header));
          std::cout << "Version is " << map.header.version << "\n";
          if( LOG_LUMP_OFFSETS )
          {
            for (int i = 0; i < 15; i++)
            {
                    std::cout << "lump " << i << " has length " <<  map.header.lump[i].nLength << "\n";
                    std::cout << " and offset " <<  map.header.lump[i].nOffset << "\n";
            }
          }

          // Entities
          map.entities = new char[map.header.lump[LUMP_ENTITIES].nLength];
          file.seekg(map.header.lump[LUMP_ENTITIES].nOffset);
          file.read(map.entities, map.header.lump[LUMP_ENTITIES].nLength * sizeof(char));

          // Textures
          Lump textureLumpHeader = map.header.lump[LUMP_TEXTURES];
          file.seekg(textureLumpHeader.nOffset);
          file.read(reinterpret_cast<char*>(&map.textureHeader), sizeof(map.textureHeader));

          map.textureOffsets = std::vector<uint32_t>(map.textureHeader.nMipTextures);
          file.read(reinterpret_cast<char*>(map.textureOffsets.data()), sizeof(uint32_t) * map.textureHeader.nMipTextures);

          map.textures = std::vector<MipTex>(map.textureHeader.nMipTextures);
          for( const uint32_t& offset : map.textureOffsets )
          {
               file.seekg(textureLumpHeader.nOffset + offset);
               MipTex mipTex;
               file.read(reinterpret_cast<char*>(&mipTex), sizeof(mipTex));
               map.textures.emplace_back(mipTex);
          }

          if( LOG_TEXTURES )
          {
            std::cout << "Amount of textures in map: " << map.textureHeader.nMipTextures << "\n";
            for ( const MipTex& mipTex : map.textures )
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
          // TODO: Load texture data

          // Texture infos
          Lump textureInfoLump = map.header.lump[LUMP_TEXINFO];
          int numTextureInfos = textureInfoLump.nLength / sizeof( Textureinfo );
          file.seekg( textureInfoLump.nOffset );
          map.textureInfos = std::vector<Textureinfo>{};
          map.textureInfos.reserve( numTextureInfos );
          for( int i = 0; i < numTextureInfos; i++ )
          {
               Textureinfo texinfo{};
               file.read(reinterpret_cast<char*>(&texinfo), sizeof(texinfo));
               map.textureInfos.push_back(texinfo);
          }

          if( LOG_TEXINFO )
          {
               std::cout << "Number of texture-info-structs: " << numTextureInfos << "\n";

               for( const auto& texinfo : map.textureInfos )
               {
                    std::cout << "Texinfo vS: " << texinfo.vS.x << " " << texinfo.vS.y << " " << texinfo.vS.z << "\n";
                    std::cout << "Texinfo vT: " << texinfo.vT.x << " " << texinfo.vT.y << " " << texinfo.vT.z << "\n";
                    std::cout << "Texinfo SShift: " << texinfo.fSShift << "\n";
                    std::cout << "Texinfo TShift: " << texinfo.fTShift << "\n";
                    std::cout << "Texinfo miptex index: " << texinfo.iMiptex << "\n";
               }
          }

          // Vertices
          Lump verticesLump = map.header.lump[LUMP_VERTICES];
          int numVertices = verticesLump.nLength / sizeof( ValveVector3d );

          map.vertices = std::vector<ValveVector3d>();

          file.seekg(verticesLump.nOffset);
          for (int i = 0; i < numVertices; i++ )
          {
               ValveVector3d vector{};
               file.read(reinterpret_cast<char*>(&vector), sizeof(ValveVector3d));
               map.vertices.push_back(vector);
          }

          if( LOG_VERTICES )
          {
               std::cout << "Number of vertices in map: " << numVertices << "\n";
               if (numVertices > 0) {
                    std::cout << "First vertex: " << map.vertices[0].x << ", " << map.vertices[0].y << ", " << map.vertices[0].z << "\n";
               }
          }

          // Faces
          Lump faceLump = map.header.lump[LUMP_FACES];
          int numFaces = faceLump.nLength / sizeof( Face );
          map.faces = std::vector<Face>();

          file.seekg(faceLump.nOffset);
          for (int i = 0; i < numFaces; i++ )
          {
               Face face{};
               file.read(reinterpret_cast<char*>(&face), sizeof(Face));
               map.faces.push_back(face);
          }


          if( LOG_FACES )
          {
               std::cout << "Number of faces in map: " << numFaces << "\n";
               if (numFaces > 0) {
                    std::cout << "First face has " << map.faces[0].nEdges << " edges, iFirstEdge=" << map.faces[0].iFirstEdge << "\n";
               }
          }

          // EDGES
          Lump edgesLump = map.header.lump[LUMP_EDGES];
          int numEdges = edgesLump.nLength / sizeof( Edge );
          map.edges = std::vector<Edge>();

          file.seekg(edgesLump.nOffset);
          for (int i = 0; i < numEdges; i++ )
          {
               Edge edge{};
               file.read(reinterpret_cast<char*>(&edge), sizeof(Edge));
               map.edges.push_back(edge);
          }


          if( LOG_EDGES )
          {
               std::cout << "Number of edges in map: " << numEdges << "\n";
               for( const Edge& edge : map.edges )
               {
                    std::cout << "Read edge with vertices " << edge.iVertex[0] << " and " << edge.iVertex[1] << "\n";
               }
          }

          // SURFEDGES
          Lump surfEdgesLump = map.header.lump[LUMP_SURFEDGES];
          int numSurfEdges = surfEdgesLump.nLength / sizeof( int32_t );
          map.surfEdges = std::vector<int32_t>();

          file.seekg(surfEdgesLump.nOffset);
          for (int i = 0; i < numSurfEdges; i++ )
          {
               int32_t surfedge;
               file.read(reinterpret_cast<char*>(&surfedge), sizeof(surfedge));
               map.surfEdges.push_back(surfedge);
          }


          if( LOG_SURFEDGES )
          {
               std::cout << "Number of surfedges in map: " << numSurfEdges << "\n";
               for( const int32_t& edge : map.surfEdges )
               {
                    std::cout << "Read surfedge " << edge << "\n";
               }
          }

          std::cout << "Size of map: " << sizeof(map) << "\n";
          return map;

    }
} // namespace BSP

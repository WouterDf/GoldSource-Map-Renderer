#include "bsp.h"
#include <cstdint>
#include <fstream>
#include <glm/fwd.hpp>
#include <iostream>
#include <ostream>
#include <simage.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <vector>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "assetloader.h"
namespace AssetLoader {
     std::filesystem::path getExecutablePath()
     {
        #ifdef _WIN32
        #include <windows.h>
            wchar_t buffer[MAX_PATH];
            GetModuleFileNameW(nullptr, buffer, MAX_PATH);
            return std::filesystem::path(buffer).parent_path();
        #endif

        #ifdef __linux__
        #include <unistd.h>
        #include <limits.h>
            char buffer[PATH_MAX];
            ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
            buffer[len] = '\0';
            return std::filesystem::path(buffer).parent_path();
        #endif

        #ifdef __APPLE__
            uint32_t size = 0;
            _NSGetExecutablePath(nullptr, &size); // get required size

            std::string buffer(size, '\0');
            _NSGetExecutablePath(buffer.data(), &size);

            return std::filesystem::path(buffer).parent_path();
        #endif
     }

     std::filesystem::path getAssetPath()
     {
          return getExecutablePath() / "assets";
     }

     std::string readAssetToString(std::filesystem::path relativePath)
     {
          std::ifstream file(getAssetPath() / relativePath, std::ios::in | std::ios::binary);
          if( !file )
          {
               throw std::runtime_error("Failed to open file.");
          }

          std::ostringstream output;
          output<< file.rdbuf();
          return output.str();
     }

     unsigned char* readImage(std::filesystem::path relativePath, int* width, int* height, int* nChannels)
     {
          std::filesystem::path imagePath = getAssetPath() / relativePath;
          return simage_read_image(imagePath.c_str(), width, height, nChannels);
     }


     BSP::BSP readBsp(std::filesystem::path relativePath)
     {
          const bool LOG_LUMP_OFFSETS = false;
          const bool LOG_TEXTURES = false;
          const bool LOG_VERTICES = true;
          const bool LOG_FACES = true;
          const bool LOG_EDGES = false;
          const bool LOG_SURFEDGES = false;
          const bool LOG_TEXINFO = false;

          std::filesystem::path mapPath = getAssetPath() / relativePath;
          std::ifstream file(mapPath, std::ios::binary);
          if( !file )
          {
               std::cerr << "BSP file not found: " << mapPath.string() << "\n";
          } else {
               std::cout << "BSP file loading: " << mapPath.string() << "\n";
          }

          BSP::BSP map{};
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
          map.entities = new char[map.header.lump[BSP::LUMP_ENTITIES].nLength];
          file.seekg(map.header.lump[BSP::LUMP_ENTITIES].nOffset);
          file.read(map.entities, map.header.lump[BSP::LUMP_ENTITIES].nLength * sizeof(char));

          // Textures
          BSP::Lump textureLumpHeader = map.header.lump[BSP::LUMP_TEXTURES];
          file.seekg(textureLumpHeader.nOffset);
          file.read(reinterpret_cast<char*>(&map.textureHeader), sizeof(map.textureHeader));

          map.textureOffsets = std::vector<uint32_t>(map.textureHeader.nMipTextures);
          file.read(reinterpret_cast<char*>(map.textureOffsets.data()), sizeof(uint32_t) * map.textureHeader.nMipTextures);

          map.textures = std::vector<BSP::MipTex>(map.textureHeader.nMipTextures);
          for( const uint32_t& offset : map.textureOffsets )
          {
               file.seekg(textureLumpHeader.nOffset + offset);
               BSP::MipTex mipTex;
               file.read(reinterpret_cast<char*>(&mipTex), sizeof(mipTex));
               map.textures.emplace_back(mipTex);
          }

          if( LOG_TEXTURES )
          {
            std::cout << "Amount of textures in map: " << map.textureHeader.nMipTextures << "\n";
            for ( const BSP::MipTex& mipTex : map.textures )
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
          BSP::Lump textureInfoLump = map.header.lump[BSP::LUMP_TEXINFO];
          int numTextureInfos = textureInfoLump.nLength / sizeof( BSP::Textureinfo );
          file.seekg( textureInfoLump.nOffset );
          map.textureInfos = std::vector<BSP::Textureinfo>{};
          map.textureInfos.reserve( numTextureInfos );
          for( int i = 0; i < numTextureInfos; i++ )
          {
               BSP::Textureinfo texinfo{};
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
          BSP::Lump verticesLump = map.header.lump[BSP::LUMP_VERTICES];
          int numVertices = verticesLump.nLength / sizeof( BSP::ValveVector3d );

          map.vertices = std::vector<BSP::ValveVector3d>();

          file.seekg(verticesLump.nOffset);
          for (int i = 0; i < numVertices; i++ )
          {
               BSP::ValveVector3d vector{};
               file.read(reinterpret_cast<char*>(&vector), sizeof(BSP::ValveVector3d));
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
          BSP::Lump faceLump = map.header.lump[BSP::LUMP_FACES];
          int numFaces = faceLump.nLength / sizeof( BSP::Face );
          map.faces = std::vector<BSP::Face>();

          file.seekg(faceLump.nOffset);
          for (int i = 0; i < numFaces; i++ )
          {
               BSP::Face face{};
               file.read(reinterpret_cast<char*>(&face), sizeof(BSP::Face));
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
          BSP::Lump edgesLump = map.header.lump[BSP::LUMP_EDGES];
          int numEdges = edgesLump.nLength / sizeof( BSP::Edge );
          map.edges = std::vector<BSP::Edge>();

          file.seekg(edgesLump.nOffset);
          for (int i = 0; i < numEdges; i++ )
          {
               BSP::Edge edge{};
               file.read(reinterpret_cast<char*>(&edge), sizeof(BSP::Edge));
               map.edges.push_back(edge);
          }


          if( LOG_EDGES )
          {
               std::cout << "Number of edges in map: " << numEdges << "\n";
               for( const BSP::Edge& edge : map.edges )
               {
                    std::cout << "Read edge with vertices " << edge.iVertex[0] << " and " << edge.iVertex[1] << "\n";
               }
          }

          // SURFEDGES
          BSP::Lump surfEdgesLump = map.header.lump[BSP::LUMP_SURFEDGES];
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
} // namespace AssetLoader

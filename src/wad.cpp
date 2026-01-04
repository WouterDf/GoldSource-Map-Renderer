
#include "wad.h"
#include "assetloader.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sys/_types/_u_int16_t.h>

namespace WAD {
     WAD::WAD(std::string filename)
     {
          this->m_filename = filename;
          this->m_info = ParseWADFile(AssetLoader::ReadWAD(filename));
     }

     

     WADInfo WAD::ParseWADFile(std::ifstream file)
    {
         const bool LOG_INFO_HEADER = false;
         const bool LOG_DIR_ENTRIES = false;

         WADInfo wad{};

         // Info header
         InfoHeader infoHeader{};
         file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
         wad.infoHeader = infoHeader;

         if( LOG_INFO_HEADER )
         {
            std::cout << "======== Start parsing wadfile" << "\n";
            std::cout << "WAD identification: " << infoHeader.identification << "\n";
            std::cout << "WAD num lumps: " << infoHeader.numlumps << "\n";
            std::cout << "Info table offset: " << infoHeader.infotableofs << "\n";
         }

         // Dir Entries
         wad.dirEntries = std::vector<DirEntry>{};
         wad.dirEntries.reserve(infoHeader.numlumps);
         file.seekg(infoHeader.infotableofs);
         for( int i = 0; i < infoHeader.numlumps; i++ )
         {
              DirEntry entry{};
              file.read(reinterpret_cast<char*>(&entry), sizeof(entry));
              wad.dirEntries.push_back(entry);
         }

         if( LOG_DIR_ENTRIES )
         {
              for( const auto& entry: wad.dirEntries )
              {
                   std::cout << "Dir entry name: " << entry.szName << "\n";
                   std::cout << "offset: " << entry.nFilePos << "\n";
                   std::cout << "disk size: " << entry.nDiskSize << "\n";
                   std::cout << "uncompressed size: " << entry.nDiskSize << "\n";
              }
         }
         return wad;
    }
    bool WAD::Contains(std::string textureName) {
        const auto found = std::find_if(
            m_info.dirEntries.begin(),
            m_info.dirEntries.end(),
            [textureName](DirEntry entry){
                return textureName == entry.szName;
            });

        return found != m_info.dirEntries.end();
    }

    // Get WADInfo for a texture name
     DirEntry WAD::GetInfo(std::string textureName)
     {
        const auto found = std::find_if(
            m_info.dirEntries.begin(),
            m_info.dirEntries.end(),
            [textureName](DirEntry entry){
                return textureName == entry.szName;
            });

        return *found;
     }


    // Load raw texture data
     std::vector<uint8_t> WAD::LoadTexture(std::string textureName, unsigned int* width, unsigned int* height)
    {
         const bool LOG_INFO = false;
         const bool LOG_PALETTE = false;
         const bool LOG_RGBA = false;
         
         DirEntry entry = GetInfo(textureName);
         auto file = AssetLoader::ReadWAD(m_filename);
         std::cout << "Reading texture: " << textureName << "\n";

         file.seekg(entry.nFilePos);
         MipTex tex{};
         file.read(reinterpret_cast<char*>(&tex), sizeof(tex));

                  unsigned int mip0size = tex.width * tex.height;
         std::vector<uint8_t> mip0;
         mip0.resize(mip0size);

         file.seekg(entry.nFilePos + tex.offsets[0]);
         file.read(reinterpret_cast<char*>(mip0.data()), mip0size);

         if( LOG_INFO )
         {
            std::cout << "name: " << tex.name << "\n";
            std::cout << "width: " << tex.width << "\n";
            std::cout << "height: " << tex.height << "\n";
            std::cout << "offset0: " << tex.offsets[0] << "\n";
            std::cout << "offset3: " << tex.offsets[3] << "\n";
            std::cout << "mip0 size:" << mip0.size() << "\n";
         }

         uint32_t paletteOffset = entry.nFilePos // miptex offset
              + tex.offsets[3] // mip3 offset
              + ( tex.width / 8 ) * (tex.height / 8); // sizeof mip3

         u_int16_t numPalleteColors;

         file.seekg(paletteOffset);
         file.read(reinterpret_cast<char*>(&numPalleteColors), sizeof(numPalleteColors));

         std::cout << "Number of palette colors: " << numPalleteColors << "\n";
         std::vector<uint8_t> palette;
         palette.resize(numPalleteColors * 3);
         file.read(reinterpret_cast<char*>(palette.data()), numPalleteColors * 3);

         if( LOG_PALETTE )
         {
            for( int i = 0; i < numPalleteColors; i++ )
            {
                std::cout << "Palette color " << i << " R: "
                            << int(palette[i*3 + 0])
                            << " G: " << int(palette[i*3 +1])
                            << " B: " << int(palette.at(i*3 + 2))
                            << "\n";
            }
         }

         // Construct RGBA
         std::vector<uint8_t> rgba;
         uint32_t rgbaSize = tex.width * tex.height * 4;
         rgba.reserve(rgbaSize);
         for( const auto& index : mip0 )
         {
              uint8_t red = palette[index*3];
              uint8_t blue = palette[index*3+1];
              uint8_t green = palette[index*3+2];
              uint8_t alpha = 255;
              rgba.push_back(red);
              rgba.push_back(blue);
              rgba.push_back(green);
              rgba.push_back(alpha);
         }

         if( LOG_RGBA )
         {
            for (int i = 0; i < 32; i++)
            {
                std::cout << "RGBA value for pixel " << i
                            << " R: " << int(rgba[4*i])
                            << " G: " << int(rgba[4*i+1])
                            << " B: " << int(rgba[4*i+2])
                            << " A: " << int(rgba[4*i+3])
                            << "\n";
            }
         }
         *width = tex.width;
         *height = tex.height;
         return rgba;
    }

} // namespace WAD

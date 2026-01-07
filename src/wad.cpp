#include "wad.h"
#include "assetloader.h"
#include "bsp.h"
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace WAD {
     WAD::WAD(const std::string& filename) : m_filename( filename )
     {
          this->m_info = ParseWADFile(AssetLoader::ReadWAD(filename));
     }

    /**
    * Texture names can start with a prefix (NAME -> +0NAME or NAME -> -5NAME). These
    * prefixes conflict when the renderer does texture lookups.
    */
    void NormalizeTextureName(char* name)
    {
        std::string stdName = name;
        if( stdName.size() > 2 && (stdName[0] == '+' || stdName[0] == '-' ) && std::isdigit(stdName[1]) )
        {
             stdName = stdName.substr(2); 
        }
        std::transform( stdName.begin(), stdName.end(), stdName.begin(), ::toupper );
        std::strncpy(name, stdName.c_str(), BSP::MAXTEXTURENAME - 1);
        name[BSP::MAXTEXTURENAME - 1] = '\0';
    }

     WADInfo WAD::ParseWADFile(std::ifstream file)
    {
         const bool LOG_INFO_HEADER = false;
         const bool LOG_DIR_ENTRIES = false;
         const bool LOG_DIR_ENTRY_NAMES = false;

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
                NormalizeTextureName(entry.szName);
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

        if( LOG_DIR_ENTRY_NAMES )
         {
              for( const auto& entry: wad.dirEntries )
              {
                   std::cout << "Texture name: " << entry.szName << "\n";
              }
         }
         return wad;
    }

    bool WAD::Contains(const std::string& textureName) {
        const auto found = std::find_if(
            m_info.dirEntries.begin(),
            m_info.dirEntries.end(),
            [textureName](DirEntry entry){
                return textureName == entry.szName;
            });

        return found != m_info.dirEntries.end();
    }

    // Get WADInfo for a texture name
     DirEntry WAD::GetInfo(const std::string& textureName)
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
     std::vector<uint8_t> WAD::LoadTexture(
          const std::string& textureName,
          unsigned int* width,
          unsigned int* height)
    {
         const bool LOG_INFO = false;
         const bool LOG_PALETTE = false;
         const bool LOG_RGBA = false;
         
         DirEntry entry = GetInfo(textureName);
         auto file = AssetLoader::ReadWAD(m_filename);

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

     WADArchive::WADArchive(const std::vector<std::string>& filenames) {
          for( const auto& filename : filenames )
          {
               this->m_wads.push_back(WAD{filename});
          }
     }

    // Check if WAD-file contains a texture by name
     bool WADArchive::Contains(const std::string& textureName)
     {
          const auto& found = std::find_if(
               this->m_wads.begin(),
               this->m_wads.end(),
               [textureName](WAD wad) {
                    return wad.Contains(textureName);
                });

          return found != this->m_wads.end();
     }

    // Load raw texture data
    std::vector<uint8_t> WADArchive::LoadTexture(
          const std::string& textureName,
          unsigned int* width,
          unsigned int* height)
    {
        WAD* wad = FindWAD(textureName);
        return wad->LoadTexture(textureName, width, height);
    }

    WAD* WADArchive::FindWAD(const std::string& textureName)
    {
        if( !this->Contains(textureName) )
        {
            throw std::runtime_error("Tried to load a texture that is not available.");
        }

        const auto found = std::find_if(
               this->m_wads.begin(),
               this->m_wads.end(),
               [textureName](WAD wad) {
                    return wad.Contains(textureName);
                });

        return &*found;
    }


} // namespace WAD

// end of wad.cpp

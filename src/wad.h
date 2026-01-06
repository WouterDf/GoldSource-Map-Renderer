#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <sys/_types/_u_int32_t.h>
#include <vector>

#include "bsp.h"

namespace WAD {

    // WAD internals
    struct InfoHeader
    {
        char identification[3]; // should be WAD2/WAD3 or 2DAW/3DAW
        uint32_t numlumps;
        uint32_t infotableofs;
    };

    struct DirEntry {
        int32_t nFilePos;            // offset in WAD
        int32_t nDiskSize;           // size in file
        int32_t nSize;               // uncompressed size
        int8_t nType;                // type of entry
        bool bCompression;           // 0 if none
        int16_t nDummy;              // not used
        char szName[BSP::MAXTEXTURENAME]; // must be null terminated
    };

    struct WADInfo {
        InfoHeader infoHeader;
        std::vector<DirEntry> dirEntries;
        bool Contains(std::string textureName);
    };

    struct MipTex {
        char name[BSP::MAXTEXTURENAME];
        uint32_t width;
        uint32_t height;
        uint32_t offsets[BSP::MIPLEVELS];
    };
    // WAD internals end

    /**
    * Texture names can start with a prefix (NAME -> +0NAME or NAME -> -5NAME). These
    * prefixes conflict when the renderer does texture lookups.
    */
    void NormalizeTextureName(char* name);

    /**
    * Represents a single WAD-file.
    */
    class WAD {
    public:

         // Constructor (reads WAD-info from disk).
         explicit WAD(const std::string& filename);

         // Check if WAD-file contains a texture by name
         bool Contains(const std::string& textureName);

         // Load raw texture data
         std::vector<uint8_t> LoadTexture(
              const std::string& textureName,
              unsigned int* width,
              unsigned int* height);

    private:
         std::string m_filename;

         WADInfo m_info;

         WADInfo ParseWADFile(std::ifstream relativePath);

         // Get DirEntry (info) for a texture name
         DirEntry GetInfo(const std::string& textureName);
    };

     /**
      * Represents all WAD files for a game.
      */
     class WADArchive {
     public:
          // Constructor (reads WAD-info from disk).
          explicit WADArchive(const std::vector<std::string>& filenames);

          // Check if WAD-file contains a texture by name
          bool Contains(const std::string& textureName);

          // Load raw texture data
          std::vector<uint8_t> LoadTexture(
               const std::string& textureName,
               unsigned int* width,
               unsigned int* height);

     private:

          // Find the WAD with textureName
          WAD* FindWAD(const std::string& textureName);

          std::vector<WAD> m_wads;
     };

} // namespace WAD

// end of wad.h

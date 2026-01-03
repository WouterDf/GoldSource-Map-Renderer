#pragma once

#include <cstdint>
#include <fstream>
#include <sys/_types/_u_int32_t.h>
#include <vector>

#define MAXTEXTURENAME 16
#define MIPLEVELS 4

namespace WAD {
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
        char szName[MAXTEXTURENAME]; // must be null terminated
    };

    struct WADInfo {
         InfoHeader infoHeader;
         std::vector<DirEntry> dirEntries;
         bool Contains(std::string textureName);
    };

    struct MipTex {
         char name[MAXTEXTURENAME];
         uint32_t width;
         uint32_t height;
         uint32_t offsets[MIPLEVELS];
    };

    // Represents a single WAD-file
    class WAD {
    public:
         //Constructor
         WAD(std::string filename);

         // Check if WAD-file contains a texture by name
         bool Contains(std::string textureName);

                  // Load raw texture data
         std::vector<uint8_t> LoadTexture(std::string textureName);

    private:
         std::string m_filename;

         WADInfo m_info;

         WADInfo ParseWADFile(std::ifstream relativePath);

         // Get DirEntry (info) for a texture name
         DirEntry GetInfo(std::string textureName);
    };

} // namespace WAD

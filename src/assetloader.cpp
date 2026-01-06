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

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "bsp.h"
#include "wad.h"
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


     std::ifstream ReadFile(std::filesystem::path relativePath)
     {
          std::filesystem::path absolutePath = getAssetPath() / relativePath;
          std::ifstream file(absolutePath, std::ios::binary);
          if( !file )
          {
               std::cerr << "File not found: " << absolutePath.string() << "\n";
          } else {
               std::cout << "File file loading: " << absolutePath.string() << "\n";
          }

          return std::move(file);
     }

     std::ifstream ReadWAD(std::filesystem::path relativePath)
     {
          std::filesystem::path mapPath = getAssetPath() / relativePath;
          std::ifstream file(mapPath, std::ios::binary);
          if( !file )
          {
               std::cerr << "WAD file not found: " << mapPath.string() << "\n";
          } else {
               std::cout << "WAD file loading: " << mapPath.string() << "\n";
          }

          return std::move(file);
     }
} // namespace AssetLoader

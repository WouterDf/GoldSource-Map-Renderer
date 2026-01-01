#pragma once

#include <cstdio>
#include <fstream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <filesystem>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

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
} // namespace AssetLoader

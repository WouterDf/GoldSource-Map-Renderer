#include <algorithm>
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
#elif defined(__linux__)
#include <unistd.h>
#include <limits.h>
#endif

#include "assetloader.h"

namespace AssetLoader {
std::filesystem::path GetExecutablePath()
{
#ifdef _WIN32
#include <windows.h>
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path();
#endif

#ifdef __linux__
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

std::filesystem::path GetAssetPath()
{
    return GetExecutablePath() / "assets";
}

std::string ReadAssetToString(std::filesystem::path relativePath)
{
    std::ifstream file(GetAssetPath() / relativePath, std::ios::in | std::ios::binary);
    if( !file )
    {
        throw std::runtime_error("Failed to open file.");
    }

    std::ostringstream output;
    output<< file.rdbuf();
    return output.str();
}

unsigned char* ReadImage(std::filesystem::path relativePath, int* width, int* height, int* nChannels)
{
    std::filesystem::path imagePath = GetAssetPath() / relativePath;
    return simage_read_image(imagePath.c_str(), width, height, nChannels);
}


std::ifstream ReadFile(std::filesystem::path relativePath)
{
    std::filesystem::path absolutePath = GetAssetPath() / relativePath;
    std::ifstream file(absolutePath, std::ios::binary);
    if( !file )
    {
        std::cerr << "File not found: " << absolutePath.string() << "\n";
    } else {
        std::cout << "File file loading: " << absolutePath.string() << "\n";
    }

    return std::move(file);
}

std::vector<std::filesystem::path> GetFilesWithExtenstion(std::string extension)
{
    std::vector<std::filesystem::path> paths;

    auto it = std::filesystem::recursive_directory_iterator(GetAssetPath());
    
    for( const auto& entry : it )
    {
        std::string entryExtension = entry.path().extension().string();
        bool equal = entryExtension.size() == extension.size()
                     && std::equal(entryExtension.begin(),
                                   entryExtension.end(),
                                   extension.begin(),
                                   [](auto a, auto b){
                                       return std::tolower(a)==std::tolower(b);
                                   });

        if( equal )
        {
            paths.push_back( entry.path() );
        }
    };

    return paths;
};

} // namespace AssetLoader

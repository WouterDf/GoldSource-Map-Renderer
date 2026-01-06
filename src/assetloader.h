#pragma once
#include <string>
#include <filesystem>

namespace AssetLoader {

     std::filesystem::path getExecutablePath();

     std::filesystem::path getAssetPath();
     
     std::string readAssetToString(std::filesystem::path relativePath);

     unsigned char* readImage(std::filesystem::path relativePath, int* width, int* height, int* nChannels);

     std::ifstream ReadFile(std::filesystem::path relativePath);

     std::ifstream ReadWAD(std::filesystem::path relativePath);
     
} // namespace AssetLoader

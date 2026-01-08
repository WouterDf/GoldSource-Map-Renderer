#pragma once
#include <string>
#include <filesystem>

namespace AssetLoader {

std::filesystem::path GetExecutablePath();

std::filesystem::path GetAssetPath();
     
std::string ReadAssetToString(std::filesystem::path relativePath);

unsigned char* ReadImage(std::filesystem::path relativePath, int* width, int* height, int* nChannels);

std::ifstream ReadFile(std::filesystem::path relativePath);

std::vector<std::filesystem::path> GetFilesWithExtenstion(std::string extension);

} // namespace AssetLoader

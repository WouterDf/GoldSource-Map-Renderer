#include "configuration.h"
#include "assetloader.h"
#include <filesystem>
#include <fstream>
#include <inipp.h>
#include <stdexcept>

Configuration::Configuration()
{
     const std::string FILE = "config.ini";
     std::filesystem::path path = AssetLoader::getAssetPath() / FILE;
     std::ifstream input(path);
     inipp::Ini<char> ini;
     ini.parse(input);

     if( ini.sections.size() == 0 )
     {
          throw std::runtime_error("Could not find config file");
     }

     inipp::extract(ini.sections["user"].at("map-name"), m_map);
};


const std::string Configuration::GetMapName()
{
     return m_map;
}

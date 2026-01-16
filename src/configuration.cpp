#include "configuration.h"
#include "assetloader.h"
#include <filesystem>
#include <fstream>
#include <inipp.h>
#include <stdexcept>

Configuration::Configuration()
{
     const std::string FILE = "config.ini";
     std::filesystem::path path = AssetLoader::GetAssetPath() / FILE;
     std::ifstream input(path);
     inipp::Ini<char> ini;
     ini.parse(input);

     if( ini.sections.size() == 0 )
     {
          throw std::runtime_error("Could not find config file");
     }

     inipp::extract(ini.sections["user"].at("map-name"), m_map);

     float camStartX;
     float camStartY;
     float camStartZ;

     inipp::extract(ini.sections["user"].at("camera-start-x"), camStartX);
     inipp::extract(ini.sections["user"].at("camera-start-y"), camStartY);
     inipp::extract(ini.sections["user"].at("camera-start-z"), camStartZ);

     m_cameraStartPosition = glm::vec3(camStartX, camStartY, camStartZ);


     inipp::extract(ini.sections["user"].at("enable-light-maps"), m_enableLightMaps);
};

const std::string Configuration::GetMapName()
{
     return m_map;
}

const glm::vec3 Configuration::GetCameraStartPosition()
{
    return m_cameraStartPosition;
}


const bool Configuration::GetEnableLightMaps()
{
    return m_enableLightMaps;
}

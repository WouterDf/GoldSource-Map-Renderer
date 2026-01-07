#pragma once

#include <string>

class Configuration {
public:
     Configuration();

     const std::string GetMapName();

private:
     std::string m_map;
};

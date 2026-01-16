#pragma once

#include <glm/ext/vector_float3.hpp>
#include <string>

class Configuration {
public:
    Configuration();

    const std::string GetMapName();

    const glm::vec3 GetCameraStartPosition();

    const bool GetEnableLightMaps();

private:
    std::string m_map;
    glm::vec3 m_cameraStartPosition;
    bool m_enableLightMaps;
};

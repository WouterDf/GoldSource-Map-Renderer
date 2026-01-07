#pragma once

#include "bsprenderbatch.h"
#include "bsprenderer.h"
#include <vector>

class WorldGeometry
{
public:
     explicit WorldGeometry(BSPRenderer* renderer);
     void Load(BSP::BSP& bsp);
     void DrawFrame();

private:
     std::vector<BSPDrawCall> m_drawCalls;
     BSPRenderer* m_renderer;
};

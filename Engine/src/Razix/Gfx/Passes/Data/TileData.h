#pragma once

#include <glm/glm.hpp>

struct TileData
{
    float3 MinCorner;
    f32       _padding;
    float3 GridSize;
    f32       CellSize;
};
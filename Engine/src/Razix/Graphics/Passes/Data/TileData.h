#pragma once

#include <glm/glm.hpp>

struct TileData
{
    glm::vec3 MinCorner;
    float     _padding;
    glm::vec3 GridSize;
    float     CellSize;
};
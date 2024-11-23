#pragma once

#include <glm/glm.hpp>

struct TileData
{
    glm::vec3 MinCorner;
    f32     _padding;
    glm::vec3 GridSize;
    f32     CellSize;
};
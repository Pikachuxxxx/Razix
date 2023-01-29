#pragma once

#include "Razix/Scene/RZSceneCamera.h"

#include <cstdint>

struct FrameBlock
{
    float                time;
    float                deltaTime;
    glm::uvec2           resolution;
    Razix::RZSceneCamera camera;
    uint32_t             renderFeatures;
    uint32_t             debugFlags;
};
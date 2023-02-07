#pragma once

#include "Razix/Scene/RZSceneCamera.h"

#include <cstdint>

#include "Razix/Graphics/FrameGraph/RZFrameGraphResource.h"

struct FrameData
{
    Razix::Graphics::FrameGraph::RZFrameGraphResource frameData;
};

struct GPUCameraFrameData
{
    alignas(16) glm::mat4 projection;
    alignas(16) glm::mat4 inversedProjection;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 inversedView;
    alignas(4) float fov;
    alignas(4) float nearPlane;
    alignas(4) float farPlane;
    alignas(4) float _padding;
};

struct GPUFrameData
{
    float                time;
    float                deltaTime;
    glm::uvec2           resolution;
    GPUCameraFrameData   camera;
    uint32_t             renderFeatures;
    uint32_t             debugFlags;
};
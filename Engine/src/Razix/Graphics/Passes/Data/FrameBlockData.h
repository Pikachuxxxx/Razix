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
    alignas(4) f32 fov;
    alignas(4) f32 nearPlane;
    alignas(4) f32 farPlane;
    alignas(4) f32 _padding;
};

struct GPUFrameData
{
    f32                time;
    f32                deltaTime;
    glm::uvec2         resolution;
    GPUCameraFrameData camera;
    u32                renderFeatures;
    u32                debugFlags;
};
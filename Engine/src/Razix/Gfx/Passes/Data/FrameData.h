#pragma once

#include "Razix/Scene/RZSceneCamera.h"

#include <cstdint>

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

struct FrameData
{
    Razix::Gfx::RZFrameGraphResource frameData;
};

struct GPUCameraFrameData
{
    alignas(16) float4x4 projection;
    alignas(16) float4x4 inversedProjection;
    alignas(16) float4x4 view;
    alignas(16) float4x4 inversedView;
    alignas(16) float4x4 prevViewProj;
    alignas(4) f32 fov;
    alignas(4) f32 nearPlane;
    alignas(4) f32 farPlane;
    alignas(4) f32 _padding;
};

struct GPUFrameData
{
    f32                time;
    f32                deltaTime;
    uint2         resolution;
    GPUCameraFrameData camera;
    u32                renderFeatures;
    u32                debugFlags;
    float2          jitterTAA;
    float2          previousJitterTAA;
};
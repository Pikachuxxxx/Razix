#pragma once

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

struct CSMData
{
    Razix::Gfx::RZFrameGraphResource cascadedShadowMaps;
    Razix::Gfx::RZFrameGraphResource viewProjMatrices;
};

constexpr u32 kShadowMapSize = 4096;
constexpr u32 kNumCascades   = 4;

static_assert(kNumCascades <= 4);

constexpr f32 kSplitLambda = 0.81f;

// clang-format off
const float4x4 kBiasMatrix
{
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
};
// clang-format on

struct alignas(4) CascadesMatrixData
{
    float4 splitDepth                     = {};
    float4x4 viewProjMatrices[kNumCascades] = {};
};

struct alignas(16) Cascade
{
    f32       splitDepth     = 0.0f;
    float4x4 viewProjMatrix = float4x4(1.0f);
};
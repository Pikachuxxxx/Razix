#pragma once

#include "Razix/Graphics/FrameGraph/RZFrameGraphResource.h"

struct CSMData
{
    Razix::Graphics::FrameGraph::RZFrameGraphResource cascadedShadowMaps;
    Razix::Graphics::FrameGraph::RZFrameGraphResource viewProjMatrices;
};

constexpr u32 kShadowMapSize = 4096;
constexpr u32 kNumCascades   = 4;

static_assert(kNumCascades <= 4);

// clang-format off
const glm::mat4 kBiasMatrix
{
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
};
// clang-format on

struct CascadesMatrixData
{
    f32       splitDepth[kNumCascades];    // This is glm::vec4 cause of max cascades is 4
    glm::mat4 viewProjMatrices[kNumCascades];
};

struct alignas(16) Cascade
{
    f32       splitDepth;
    glm::mat4 viewProjMatrix;
};
#pragma once

#include "Razix/Graphics/FrameGraph/RZFrameGraphResource.h"

struct ShadowMapData
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

struct CasdacesUBOData
{
    glm::vec4 splitDepth;    // This is glm::vec4 cause of max cascades is 4
    glm::mat4 viewProjMatrices[kNumCascades];
};

struct Cascade
{
    f32     splitDepth;
    glm::mat4 viewProjMatrix;
};
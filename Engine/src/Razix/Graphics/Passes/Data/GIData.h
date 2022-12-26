#pragma once

#include "Razix/Graphics/FrameGraph/RZFrameGraphResource.h"

constexpr auto kRSMResolution           = 512;
constexpr auto kNumVPL                  = kRSMResolution * kRSMResolution;
constexpr auto kLPVResolution           = 32;
constexpr auto kFirstFreeTextureBinding = 0;
constexpr auto kDefaultNumPropagations  = 6;

struct ReflectiveShadowMapData
{
    Razix::Graphics::FrameGraph::RZFrameGraphResource depth;
    Razix::Graphics::FrameGraph::RZFrameGraphResource position;
    Razix::Graphics::FrameGraph::RZFrameGraphResource normal;
    Razix::Graphics::FrameGraph::RZFrameGraphResource flux;
};
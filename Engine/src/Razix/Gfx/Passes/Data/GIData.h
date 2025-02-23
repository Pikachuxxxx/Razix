#pragma once

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

constexpr auto kRSMResolution           = 512;
constexpr auto kNumVPL                  = kRSMResolution * kRSMResolution;
constexpr auto kLPVResolution           = 32;
constexpr auto kFirstFreeTextureBinding = 0;
constexpr auto kDefaultNumPropagations  = 6;

struct ReflectiveShadowMapData
{
    Razix::Gfx::FrameGraph::RZFrameGraphResource position;
    Razix::Gfx::FrameGraph::RZFrameGraphResource normal;
    Razix::Gfx::FrameGraph::RZFrameGraphResource flux;
    Razix::Gfx::FrameGraph::RZFrameGraphResource depth;
};

struct LightPropagationVolumesData
{
    Razix::Gfx::FrameGraph::RZFrameGraphResource r;
    Razix::Gfx::FrameGraph::RZFrameGraphResource g;
    Razix::Gfx::FrameGraph::RZFrameGraphResource b;
};

struct RadianceInjectionUBOData
{
    int       RSMResolution;
    f32       CellSize;
    char      _padding[5];
    float3 MinCorner;
    char      _padding_;
    float3 GridSize;
    //char       _padding__;
};

struct RadiancePropagationUBOData
{
    float3 GridSize;
    char      _padding;
};
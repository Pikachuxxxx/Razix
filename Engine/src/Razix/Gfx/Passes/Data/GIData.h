#pragma once

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

constexpr auto kRSMResolution           = 512;
constexpr auto kNumVPL                  = kRSMResolution * kRSMResolution;
constexpr auto kLPVResolution           = 32;
constexpr auto kFirstFreeTextureBinding = 0;
constexpr auto kDefaultNumPropagations  = 6;

struct ReflectiveShadowMapData
{
    Razix::Gfx::RZFrameGraphResource position;
    Razix::Gfx::RZFrameGraphResource normal;
    Razix::Gfx::RZFrameGraphResource flux;
    Razix::Gfx::RZFrameGraphResource depth;
};

struct LightPropagationVolumesData
{
    Razix::Gfx::RZFrameGraphResource r;
    Razix::Gfx::RZFrameGraphResource g;
    Razix::Gfx::RZFrameGraphResource b;
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
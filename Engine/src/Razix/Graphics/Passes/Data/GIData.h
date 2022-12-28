#pragma once

#include "Razix/Graphics/FrameGraph/RZFrameGraphResource.h"

constexpr auto kRSMResolution           = 512;
constexpr auto kNumVPL                  = kRSMResolution * kRSMResolution;
constexpr auto kLPVResolution           = 32;
constexpr auto kFirstFreeTextureBinding = 0;
constexpr auto kDefaultNumPropagations  = 6;

struct ReflectiveShadowMapData
{
    Razix::Graphics::FrameGraph::RZFrameGraphResource position;
    Razix::Graphics::FrameGraph::RZFrameGraphResource normal;
    Razix::Graphics::FrameGraph::RZFrameGraphResource flux;
    Razix::Graphics::FrameGraph::RZFrameGraphResource depth;
};

struct LightPropagationVolumesData
{
    Razix::Graphics::FrameGraph::RZFrameGraphResource r;
    Razix::Graphics::FrameGraph::RZFrameGraphResource g;
    Razix::Graphics::FrameGraph::RZFrameGraphResource b;
};

struct RadianceInjectionUBOData
{
    int       RSMResolution;
    float     CellSize;
    char       _padding[5];
    glm::vec3 MinCorner;
    char       _padding_;
    glm::vec3 GridSize;
    //char       _padding__;
};

struct RadiancePropagationUBOData
{
    glm::vec3 GridSize;
    char      _padding;
};
#pragma once

#include "Razix/Graphics/FrameGraph/RZFrameGraphResource.h"

struct GBufferData
{
    Razix::Graphics::FrameGraph::RZFrameGraphResource Normal;
    Razix::Graphics::FrameGraph::RZFrameGraphResource Albedo;
    Razix::Graphics::FrameGraph::RZFrameGraphResource Emissive;
    Razix::Graphics::FrameGraph::RZFrameGraphResource MetRougAOSpec;
    Razix::Graphics::FrameGraph::RZFrameGraphResource Depth;
};
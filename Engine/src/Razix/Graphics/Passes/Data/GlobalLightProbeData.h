#pragma once

#include "Razix/Graphics/FrameGraph/RZFrameGraphResource.h"

struct GlobalLightProbeData
{
    Razix::Graphics::FrameGraph::RZFrameGraphResource diffuseIrradianceMap;
    Razix::Graphics::FrameGraph::RZFrameGraphResource specularPreFilteredMap;
};
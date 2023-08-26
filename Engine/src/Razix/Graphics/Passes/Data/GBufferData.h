#pragma once

#include "Razix/Graphics/FrameGraph/RZFrameGraphResource.h"

struct GBufferData
{
    Razix::Graphics::FrameGraph::RZFrameGraphResource Normal_Metallic;
    Razix::Graphics::FrameGraph::RZFrameGraphResource Albedo_Roughness;
    Razix::Graphics::FrameGraph::RZFrameGraphResource Position_AO;
    Razix::Graphics::FrameGraph::RZFrameGraphResource Depth;
};

// Output from Fragment Shader or Output to Framebuffer attachments
//layout(location = 0) out vec4 GBuffer0;    // .rgb = Normal   .a = Metallic
//layout(location = 1) out vec4 GBuffer1;    // .rgb = Albedo   .a = Roughness
//layout(location = 2) out vec4 GBuffer2;    // .rgb = Position .a = AO
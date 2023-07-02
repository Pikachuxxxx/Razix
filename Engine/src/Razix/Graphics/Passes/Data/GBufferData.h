#pragma once

#include "Razix/Graphics/FrameGraph/RZFrameGraphResource.h"

struct GBufferData
{
    Razix::Graphics::FrameGraph::RZFrameGraphResource Normal_PosX;
    Razix::Graphics::FrameGraph::RZFrameGraphResource Albedo_PosY;
    Razix::Graphics::FrameGraph::RZFrameGraphResource Emissive_PosZ;
    Razix::Graphics::FrameGraph::RZFrameGraphResource MetRougAOAlpha;
    Razix::Graphics::FrameGraph::RZFrameGraphResource Depth;
};

// Output from Fragment Shader or Output to Framebuffer attachments
//layout(location = 0) out vec4 GBuffer0;    // .rgb = Normal .a = Position.x
//layout(location = 1) out vec4 GBuffer1;    // .rgb = Albedo .a = Position.y
//layout(location = 2) out vec4 GBuffer2;    // .rgb = Emissive .a = Position.z
//layout(location = 3) out vec4 GBuffer3;    // .r = Metallic .g = roughness .b = AO .a = alpha/opacity
#pragma once

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

struct GBufferData
{
    Razix::Gfx::FrameGraph::RZFrameGraphResource GBuffer0;
    Razix::Gfx::FrameGraph::RZFrameGraphResource GBuffer1;
    Razix::Gfx::FrameGraph::RZFrameGraphResource GBuffer2;
    Razix::Gfx::FrameGraph::RZFrameGraphResource VelocityBuffer;
    Razix::Gfx::FrameGraph::RZFrameGraphResource GBufferDepth;
};

// Output from Fragment Shader or Output to Framebuffer attachments
//layout(location = 0) out vec4 GBuffer0;       // .rgb = Normal   .a = Metallic
//layout(location = 1) out vec4 GBuffer1;       // .rgb = Albedo   .a = Roughness
//layout(location = 2) out vec4 GBuffer2;       // .rgb = Position .a = AO
//layout(location = 3) out vec2 VelocityBuffer; // .rg = Velocity
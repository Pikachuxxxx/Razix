#ifndef _FRAME_DATA_GLSL_
#define _FRAME_DATA_GLSL_
//----------------------------------------------------------------------------
// TODO: Don't redefine this, use from Engine source
struct Camera
{
    float4x4 projection;
    float4x4 inversedProjection;
    float4x4 view;
    float4x4 inversedView;
    float4x4 prevViewProj;
    float    fov;
    float    near, far;
    // Implicit padding, 4bytes
};
//----------------------------------
// TODO: Remove this struct
struct Frameframe_info
{
    float  time;
    float  deltaTime;
    uint2  resolution;
    Camera camera;
    uint   renderFeatures;
    uint   debugFlags;
    float2 jitterTAA;
    float2 previousJitterTAA;
};
//----------------------------------
#ifndef DISABLE_FRAME_DATA_BINDING
cbuffer FrameData : register(b0, space0)
{
    Frameframe_info frame_info;
};
//----------------------------------------------------------------------------
static const uint RendererFeature_Shadows    = 1 << 0;
static const uint RendererFeature_GI         = 1 << 1;
static const uint RendererFeature_IBL        = 1 << 2;
static const uint RendererFeature_SSAO       = 1 << 3;
static const uint RendererFeature_SSR        = 1 << 4;
static const uint RendererFeature_Bloom      = 1 << 5;
static const uint RendererFeature_FXAA       = 1 << 6;
static const uint RendererFeature_TAA        = 1 << 7;
static const uint RendererFeature_ImGui      = 1 << 8;
static const uint RendererFeature_Deferred   = 1 << 9;
static const uint RendererFeature_DebugDraws = 1 << 10;
//----------------------------------
// !Note: Only to be used when ENABLE_FRAME_DATA_BINDING is defined in the shader
float getTime()
{
    return frame_info.time;
}
float getDeltaTime()
{
    return frame_info.deltaTime;
}

uint2 getResolution()
{
    return frame_info.resolution;
}
float2 getTexelSize()
{
    return 1.0 / float2(frame_info.resolution);
}

float3 getCameraPosition()
{
    return frame_info.camera.inversedView[3].xyz;
}

bool hasRenderFeatures(uint f)
{
    return (frame_info.renderFeatures & f) == f;
}
bool hasDebugFlags(uint f)
{
    return (frame_info.debugFlags & f) == f;
}
//----------------------------------------------------------------------------
#endif    // DISABLE_FRAME_DATA_BINDING

float3 getCameraPosition(Camera cam)
{
    return cam.inversedView[3].xyz;
}

float4 getProjectionViewSpacePosition(float4 pos)
{
    float4x4 jitterMatrix = float4x4(
        1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, frame_info.jitterTAA.x, frame_info.jitterTAA.y, 0.0, 1.0    // translation
    );

    float4x4 jitterProj = mul(frame_info.camera.projection, jitterMatrix);

    // Final position of the vertices
    float4 worldPos = mul(frame_info.camera.view, pos);

    float4 jitterWorldPos = mul(jitterProj, worldPos);

    return jitterWorldPos;
}

#endif

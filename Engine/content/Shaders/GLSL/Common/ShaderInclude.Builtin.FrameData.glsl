#ifndef _FRAME_DATA_GLSL_
#define _FRAME_DATA_GLSL_
//----------------------------------------------------------------------------
struct Camera {
    mat4 projection;
    mat4 inversedProjection;
    mat4 view;
    mat4 inversedView;
    float fov;
    float near, far;
    // Implicit padding, 4bytes
};
//----------------------------------
// TODO: Remove this struct
struct FrameInfo {
    float time;
    float deltaTime;
    uvec2 resolution;
    Camera camera;
    uint renderFeatures;
    uint debugFlags;
    vec2 jitterTAA;
};
//----------------------------------
#ifndef DISABLE_FRAME_DATA_BINDING
layout(set = 0, binding = 0, std140) uniform FrameDataBuffer {
    FrameInfo info;
} FrameData;
//----------------------------------------------------------------------------
const uint RendererFeature_Shadows    = 1 << 0;
const uint RendererFeature_GI         = 1 << 1;
const uint RendererFeature_IBL        = 1 << 2;
const uint RendererFeature_SSAO       = 1 << 3;
const uint RendererFeature_SSR        = 1 << 4;
const uint RendererFeature_Bloom      = 1 << 5;
const uint RendererFeature_FXAA       = 1 << 6;
const uint RendererFeature_TAA        = 1 << 7;
const uint RendererFeature_ImGui      = 1 << 8;
const uint RendererFeature_Deferred   = 1 << 9;
const uint RendererFeature_DebugDraws = 1 << 10;
//----------------------------------
// !Note: Only to be used when ENABLE_FRAME_DATA_BINDING is defined in the shader
float getTime() { return FrameData.info.time; }
float getDeltaTime() { return FrameData.info.deltaTime; }

uvec2 getResolution() { return FrameData.info.resolution; }
vec2 getTexelSize() { return 1.0 / vec2(FrameData.info.resolution); }

vec3 getCameraPosition() { return FrameData.info.camera.inversedView[3].xyz; }

bool hasRenderFeatures(uint f) { return (FrameData.info.renderFeatures & f) == f; }
bool hasDebugFlags(uint f) { return (FrameData.info.debugFlags & f) == f; }
//----------------------------------------------------------------------------
#endif

vec3 getCameraPosition(Camera cam) { return cam.inversedView[3].xyz; }

vec4 getProjectionViewSpacePosition(vec4 pos)
{
    mat4 jitterMatrix = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        FrameData.info.jitterTAA.x, FrameData.info.jitterTAA.y, 0.0, 1.0  // translation 
    );

    mat4 jitterProj = FrameData.info.camera.projection * jitterMatrix;

    // Final position of the vertices
    return jitterProj * FrameData.info.camera.view * pos;
}

#endif

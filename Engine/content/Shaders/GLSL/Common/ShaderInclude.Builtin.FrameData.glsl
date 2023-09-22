#ifndef _FRAME_DATA_GLSL_
#define _FRAME_DATA_GLSL_

struct Camera {
    mat4 projection;
    mat4 inversedProjection;
    mat4 view;
    mat4 inversedView;
    float fov;
    float near, far;
    // Implicit padding, 4bytes
};

layout(set = 0, binding = 0, std140) uniform FrameDataBuffer {
    float time;
    float deltaTime;
    uvec2 resolution;
    Camera camera;
    uint renderFeatures;
    uint debugFlags;
} FrameData;

float getTime() { return FrameData.time; }
float getDeltaTime() { return FrameData.deltaTime; }

uvec2 getResolution() { return FrameData.resolution; }
vec2 getTexelSize() { return 1.0 / vec2(FrameData.resolution); }

vec3 getCameraPosition() { return FrameData.camera.inversedView[3].xyz; }

const uint RenderFeature_Shadows = 1 << 0;
const uint RenderFeature_GI = 1 << 1;
const uint RenderFeature_IBL = 1 << 2;
const uint RenderFeature_SSAO = 1 << 3;

bool hasRenderFeatures(uint f) { return (FrameData.renderFeatures & f) == f; }

const uint DebugFlag_RadianceOnly = 1 << 3;

bool hasDebugFlags(uint f) { return (FrameData.debugFlags & f) == f; }

#endif

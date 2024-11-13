#version 450

struct Camera
{
    mat4 projection;
    mat4 inversedProjection;
    mat4 view;
    mat4 inversedView;
    mat4 prevViewProj;
    float fov;
    float near;
    float far;
};

struct Frameframe_info
{
    float time;
    float deltaTime;
    uvec2 resolution;
    Camera camera;
    uint renderFeatures;
    uint debugFlags;
    vec2 jitterTAA;
    vec2 previousJitterTAA;
};

layout(set = 0, binding = 0, std140) uniform type_FrameData
{
    layout(row_major) Frameframe_info frame_info;
} FrameData;

layout(push_constant, std430) uniform type_PushConstant_ModelPushConstantData
{
    layout(row_major) mat4 worldTransform;
    layout(row_major) mat4 previousWorldTransform;
} pcData;

layout(location = 0) in vec3 in_var_POSITION;
layout(location = 0) out vec4 out_var_COLOR;
layout(location = 1) out vec2 out_var_TEXCOORD;
layout(location = 2) out vec3 out_var_NORMAL;

void main()
{
    gl_Position = ((vec4(in_var_POSITION, 1.0) * pcData.worldTransform) * FrameData.frame_info.camera.view) * FrameData.frame_info.camera.projection;
}


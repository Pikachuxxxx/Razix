#version 450

layout(set = 0, binding = 0, std140) uniform type_ViewProjectionBuffer
{
    layout(row_major) mat4 view;
    layout(row_major) mat4 proj;
} ViewProjectionBuffer;

layout(set = 0, binding = 1, std140) uniform type_ModelPushConstantData
{
    layout(row_major) mat4 worldTransform;
    layout(row_major) mat4 previousWorldTransform;
} ModelPushConstantData;

layout(location = 0) in vec3 in_var_POSITION;
layout(location = 1) in vec4 in_var_COLOR;
layout(location = 2) in vec2 in_var_TEXCOORD;
layout(location = 3) in vec3 in_var_NORMAL;
layout(location = 0) out vec4 out_var_COLOR;
layout(location = 1) out vec2 out_var_TEXCOORD;
layout(location = 2) out vec3 out_var_NORMAL;

void main()
{
    gl_Position = ((vec4(in_var_POSITION, 1.0) * ModelPushConstantData.worldTransform) * ViewProjectionBuffer.view) * ViewProjectionBuffer.proj;
    out_var_COLOR = in_var_COLOR;
    out_var_TEXCOORD = in_var_TEXCOORD;
    out_var_NORMAL = in_var_NORMAL;
}


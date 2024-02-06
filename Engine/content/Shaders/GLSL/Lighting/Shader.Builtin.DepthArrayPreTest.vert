#version 450
/*
 * Razix Engine GLSL Vertex Shader File
 * Vertex shader to render a depth texture onto a Texture2D
 */
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable
// This extension enables ussage of gl_Layer in the Vertex Shader stage itself instead of using GS 
// also needs VK_EXT_shader_viewport_index_layer device extension enabled and layersCount in VkRenderingInfo
#extension GL_ARB_shader_viewport_layer_array : enable

//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;   // this is not needed for all types of meshes, but since we use a common vertex shader we need this too
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;
//------------------------------------------------------------------------------
// Uniforms and Push Constants 
// view projection matrix
layout(set = 0, binding = 0) uniform ViewProjectionArrayUBO
{
    mat4 mat;
    int layer;
} VPArray;
layout (push_constant) uniform ModelPushConstantData{
    mat4 model;
}model_pc_data;
 //------------------------------------------------------------------------------ 
out gl_PerVertex
{
    vec4 gl_Position;
};
//------------------------------------------------------------------------------
void main()
{
    gl_Layer = VPArray.layer;
    gl_Position = VPArray.mat * model_pc_data.model * vec4(inPosition, 1.0f);
}
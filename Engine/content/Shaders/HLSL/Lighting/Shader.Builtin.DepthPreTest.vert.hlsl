#version 450
/*
 * Razix Engine HLSL Vertex Shader File
 * Vertex shader to render a depth texture onto a 2D Texture
 */
// This extension enables ussage og gl_Layer in the Vertex Shader stage itself instead of using GS 
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
layout(set = 0, binding = 0) uniform ViewProjectionSystemUBO
{
    mat4 mat;
} LightSpaceMatrix;
layout (push_constant) uniform ModelPushConstantData{
    mat4 worldTransform;
    mat4 previousWorldTransform;
}model_pc_data;
 //------------------------------------------------------------------------------ 
out gl_PerVertex
{
    vec4 gl_Position;
};
//------------------------------------------------------------------------------
void main()
{
    gl_Position = LightSpaceMatrix.mat * model_pc_data.worldTransform * vec4(inPosition, 1.0f);
}
#version 450
/*
 * Razix Engine GLSL Vertex Shader File
 * Default Vertex Shader that can be used for rendering basic geometry with vertex colors and use a texture as well
 */
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable

//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
//------------------------------------------------------------------------------
// Uniforms and Push Constants
// Thew view projection matrix
layout(set = 0, binding = 0) uniform ViewProjectionUniformBufferObject
{
    mat4 view;
	mat4 proj;
} view_proj_ubo;

// The model push constant
layout (push_constant) uniform ModelPushConstantData{
    mat4 worldTransform;
    mat4 previousWorldTransform;
}model_pc_data;
//------------------------------------------------------------------------------
// Vertex Shader Stage Output
layout(location = 0) out VSOutput
{
     vec4 fragColor;
     vec2 fragUV;
     vec3 fragNormal;
}vs_out;

out gl_PerVertex
{
    vec4 gl_Position;
};
//------------------------------------------------------------------------------
void main()
{
    // Final position of the vertices
    gl_Position = view_proj_ubo.proj * view_proj_ubo.view * model_pc_data.worldTransform * vec4(inPosition, 1.0);//vec4(inPosition, 1.0);//view_proj_ubo.proj * view_proj_ubo.view *//

    // Out from vertex shader
    vs_out.fragColor    = inColor;
	vs_out.fragUV = inTexCoord;
    vs_out.fragNormal   = inNormal;
}
//------------------------------------------------------------------------------

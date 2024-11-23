#version 450
/*
 * Razix Engine GLSL Vertex Shader File
 * Vertex shader to render a cubemap, uses a Cube mesh in vertex shader to render stuff onto
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
layout(location = 4) in vec3 inTangent;
//------------------------------------------------------------------------------
// Vertex Shader Stage Output
layout(location = 0) out VSOutput{ 
    vec3 normal;
    vec2 texCoord;
    vec3 localPos;
} vs_out;
//------------------------------------------------------------------------------
// Uniforms and Push Constants
// view projection matrix
layout(set = 0, binding = 0) uniform ViewProjectionSystemUBO
{
    mat4 view;
	mat4 proj;
} view_proj_ubo;
 //------------------------------------------------------------------------------ 
out gl_PerVertex
{
    vec4 gl_Position;
};
//------------------------------------------------------------------------------
void main()
{
    gl_Position = view_proj_ubo.proj * view_proj_ubo.view * vec4(inPosition, 1.0f);
    vs_out.localPos = inPosition;
    vs_out.texCoord = inTexCoord;
    vs_out.normal = inNormal;
}
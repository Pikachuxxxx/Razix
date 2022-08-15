#version 450
/*
 * Razix Engine GLSL Vertex Shader File
 * Shader that can be used to render a grid, can be used in Editor and for visualization
 */
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable

//------------------------------------------------------------------------------
// Vertex Input
// Since we use a plane primitive mesh for the Grid we need the same vertex layout as that of a mesh vertex shader
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
//-----------------------------------
// Uniforms and Push Constants
// The view projection matrix
layout(set = 0, binding = 0) uniform ViewProjectionUBOData
{
    mat4 view;
	mat4 proj;
} view_proj_ubo;
//------------------------------------------------------------------------------
// Vertex Shader Stage Output
layout(location = 0) out VSOutput
{
    vec3 fragPosition;
    vec2 fragTexCoord;
}vs_out;

out gl_PerVertex
{
    vec4 gl_Position;
};
//------------------------------------------------------------------------------
void main()
{
    gl_Position = view_proj_ubo.proj * view_proj_ubo.view * vec4(inPosition, 1.0);
    // Out data from vertex shader
    vs_out.fragPosition = inPosition;
	vs_out.fragTexCoord  = inTexCoord;
}
//------------------------------------------------------------------------------

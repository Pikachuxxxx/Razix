#version 450
/*
 * Razix Engine GLSL Vertex Shader File
 * Used to render a debug point
 */
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable

#include <Common/FrameData.glsl>
//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inSize;
layout(location = 3) in vec2 inTexCoord;
//------------------------------------------------------------------------------
// Uniforms and Push Constants
// view projection matrix
//------------------------------------------------------------------------------
// Vertex Shader Stage Output
layout(location = 0) out VSOutput
{
    vec4 fragColor;
    vec2 fragTexCoord;
}vs_out;

out gl_PerVertex
{
    vec4 gl_Position;
};
//------------------------------------------------------------------------------
void main()
{
    gl_Position = u_Frame.camera.projection * u_Frame.camera.view * vec4(inPosition.xyz, 1.0);
    // Out from vertex shader
    vs_out.fragColor    = inColor;
	vs_out.fragTexCoord = inTexCoord;
}
//------------------------------------------------------------------------------

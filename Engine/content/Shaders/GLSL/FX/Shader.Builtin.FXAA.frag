/*
 * Razix Engine GLSL Pixel Shader File
 * Resolves TAA and write to the history buffer
 */
#version 450 core
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable
//------------------------------------------------------------------------------
#include <Utils/ShaderInclude.Builtin.Color.glsl>
//------------------------------------------------------------------------------
// Constants and Defines

//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{
    vec2 fragUV;
}fs_in; 
//--------------------------------------------------------
layout (set = 0, binding = 0) uniform sampler2D CurrentTexture; 
//------------------------------------------------------------------------------
// Output from Fragment Shader : Write to an accumulation buffer
layout(location = 0) out vec4 outSceneColor;
//------------------------------------------------------------------------------
void main()
{
    vec2 uv = fs_in.fragUV;
    vec4 currentColor = texture(CurrentTexture, uv);
 
    outSceneColor = currentColor * 0.1f + previousColor * 0.9f;
}
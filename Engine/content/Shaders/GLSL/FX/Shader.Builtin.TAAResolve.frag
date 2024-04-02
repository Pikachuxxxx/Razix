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
// Constants and Defines

//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{
    vec2 fragUV;
}fs_in; 
//--------------------------------------------------------
layout (set = 0, binding = 0) uniform sampler2D CurrentTexture; 
layout (set = 0, binding = 1) uniform sampler2D HistoryTexture; 
//------------------------------------------------------------------------------
// Output from Fragment Shader : Write to an accumulation buffer
layout(location = 0) out vec4 outAccumulationColor;
//------------------------------------------------------------------------------
void main()
{
    vec2 uv = fs_in.fragUV;
    vec4 currentColor = texture(CurrentTexture, uv);
    vec4 previousColor = texture(HistoryTexture, uv);
 
    outAccumulationColor = mix(currentColor, previousColor, 0.9f);
}
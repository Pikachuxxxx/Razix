/*
 * Razix Engine GLSL Pixel Shader File
 * Render deferred decals onto the GBuffer
 * [Source] : https://www.gamedevs.org/uploads/screenspace-decals-space-marine.pdf
 */
#version 450 core
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable
//------------------------------------------------------------------------------
// Color Utils (Debug only)
#include <Utils/ShaderInclude.Builtin.Color.glsl>
//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{
    vec2 fragUV;
}fs_in;
//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
// @ slot #0 - .rgb = Normal   .a = Metallic
// @ slot #1 - .rgb = Albedo   .a = Roughness
// @ slot #2 - .rgb = Position .a = AO
layout (set = 0, binding = 0) uniform sampler2D gBuffer0; 
layout (set = 0, binding = 1) uniform sampler2D gBuffer1; 
layout (set = 0, binding = 2) uniform sampler2D gBuffer2; 
//------------------------------------------------------------------------------ 
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 GBuffer0;    // .rgb = Normal   .a = Metallic
layout(location = 1) out vec4 GBuffer1;    // .rgb = Albedo   .a = Roughness
layout(location = 2) out vec4 GBuffer2;    // .rgb = Position .a = AO
//------------------------------------------------------------------------------
void main()
{
    vec2 uv = fs_in.fragUV;
    
    GBuffer0 = vec4(RandomColorHash(69u), 1.0f);
}
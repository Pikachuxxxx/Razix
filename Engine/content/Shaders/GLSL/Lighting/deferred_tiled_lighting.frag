/*
 * Razix Engine GLSL Vertex Shader File
 * Pixel Shader for deferred tiled lighting
 */
#version 450 core
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable
//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{ 
    vec2 fragTexCoord;
}fs_in;
//------------------------------------------------------------------------------ 
// Uniforms and Sampler Input attachments
// GBuffer
layout(set = 0, binding = 0) uniform sampler2D sceneDepth;

layout(set = 0, binding = 1) uniform sampler2D GBuffer0; // .rgb = Normal .a = empty (1.0f)
layout(set = 0, binding = 2) uniform sampler2D GBuffer1; // .rgb = Albedo .a = empty (1.0f)
layout(set = 0, binding = 3) uniform sampler2D GBuffer2; // .rgb = Emissive .a = empty (1.0f)
layout(set = 0, binding = 4) uniform sampler2D GBuffer3; // .r = Metallic .g = roughness .b = AO .a = specular

layout(set = 0, binding = 5) uniform sampler2D BRDF;
layout(set = 0, binding = 6) uniform samplerCube IrradianceMap;
layout(set = 0, binding = 7) uniform samplerCube PrefilteredEnvMap;

layout(set = 0, binding = 8) uniform sampler2DArrayShadow CascadedShadowMaps;

layout(set = 0, binding = 9) uniform sampler3D AccumulatedSH_R;
layout(set = 0, binding = 10) uniform sampler3D AccumulatedSH_G;
layout(set = 0, binding = 11) uniform sampler3D AccumulatedSH_B;
//------------------------------------------------------------------------------ 
// Includes
#include <Lighting/Light.glsl>
#include <Lighting/PBR/IBL_AmbientLighting.glsl>
#include <Lighting/PBR/PBR_DirectLighting.glsl>

#include <Utils/Depth.glsl>
#include <Utils/Texture.glsl>
//------------------------------------------------------------------------------ 
// Functions

//------------------------------------------------------------------------------ 
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------
void main()
{

}
//------------------------------------------------------------------------------
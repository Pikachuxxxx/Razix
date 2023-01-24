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
#include <Common/FrameData.glsl>
//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{ 
    vec2 fragTexCoord;
}fs_in;
//------------------------------------------------------------------------------ 
// Uniforms and Sampler Input attachments
// GBuffer
layout(set = 1, binding = 0) uniform sampler2D SceneDepth;
             
layout(set = 1, binding = 1) uniform sampler2D GBuffer0; // .rgb = Normal .a = empty (1.0f)
layout(set = 1, binding = 2) uniform sampler2D GBuffer1; // .rgb = Albedo .a = empty (1.0f)
layout(set = 1, binding = 3) uniform sampler2D GBuffer2; // .rgb = Emissive .a = empty (1.0f)
layout(set = 1, binding = 4) uniform sampler2D GBuffer3; // .r = Metallic .g = roughness .b = AO .a = specular
             
layout(set = 1, binding = 5) uniform sampler2D BRDF;
layout(set = 1, binding = 6) uniform samplerCube IrradianceMap;
layout(set = 1, binding = 7) uniform samplerCube PrefilteredEnvMap;
             
layout(set = 1, binding = 8) uniform sampler2DArrayShadow CascadedShadowMaps;
             
layout(set = 1, binding = 9) uniform sampler3D AccumulatedSH_R;
layout(set = 1, binding = 10) uniform sampler3D AccumulatedSH_G;
layout(set = 1, binding = 11) uniform sampler3D AccumulatedSH_B;

// TODO: Add grid info for tiled lighting calculation
//------------------------------------------------------------------------------ 
// Includes
#include <Lighting/Light.glsl>
#include <Lighting/PBR/IBL_AmbientLighting.glsl>
#include <Lighting/PBR/PBR_DirectLighting.glsl>

#include <Utils/Depth.glsl>
#include <Utils/Texture.glsl>

#include <Lighting/CSM.glsl>
#include <Lighting/GI/LPV.glsl>
//------------------------------------------------------------------------------ 
// Functions
//LightContribution CalculateIndirectLight()
//{
//}
//------------------------------------------------------------------------------ 
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------
void main()
{
    // Decoding the GBuffer textures
    //---------------------------------------------------------------------------
    // Get the depth of the fragments + clipping for <= 1.0f
    const float depth = getDepth(SceneDepth, fs_in.fragTexCoord);
    if (depth >= 1.0) discard;

    const vec3 albedoColor = texture(GBuffer1, fs_in.fragTexCoord).rgb;
    const vec3 emissiveColor = texture(GBuffer2, fs_in.fragTexCoord).rgb;

    vec4 mroa = texture(GBuffer3, fs_in.fragTexCoord);

    const float metallic = mroa.r;
    const float roughness = mroa.g;
    const float ao = mroa.b;
    const float specularWeight = mroa.a;
    //---------------------------------------------------------------------------


    // Lighting Calculation
    //---------------------------------------------------------------------------
    // Get the N, L, V, H vectors for all the lighting calculation
    // N = normal (from surface)
    // L = fragment to light direction (lightDir)
    // V = fragment to eye direction (eyeViewDir)
    // H = halfway vector (between V and L)

    const vec3 fragPosViewSpace = viewPositionFromDepth(depth, fs_in.fragTexCoord);
    const vec3 fragPosWorldSpace = (u_Frame.camera.inversedView * vec4(fragPosViewSpace, 1.0)).xyz;

    vec3 N = normalize(texture(GBuffer0, fs_in.fragTexCoord).rgb);
    const vec3 V = normalize(getCameraPosition() - fragPosWorldSpace);
    const float NdotV = clamp01(dot(N, V));

    const float alphaRoughness = roughness * roughness;


    //---------------------------------------------------------------------------
}
//------------------------------------------------------------------------------  
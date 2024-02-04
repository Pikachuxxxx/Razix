/*
 * Razix Engine GLSL Vertex Shader File
 * Calcualte the Final PBR direct lighting
 */
#version 450 core
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable
//------------------------------------------------------------------------------
// Bindless Textures
//#define ENABLE_BINDLESS 1
//-------------------------------
// Material Data
#include <Material/ShaderInclude.Builtin.Material.glsl>
//-------------------------------
// Lights Data
#include <Lighting/ShaderInclude.Builtin.Light.glsl>
//-------------------------------
// PBR - BRDF helper functions
#include <Lighting/PBR/ShaderInclude.Builtin.BRDF.glsl>
#include <Lighting/PBR/ShaderInclude.Builtin.PBRDirectLighting.glsl>
#include <Lighting/ShaderInclude.Builtin.ComputeShadows.glsl>
//-------------------------------
// Color Utils (Debug only)
#include <Utils/ShaderInclude.Builtin.Color.glsl>
//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{
    vec3 fragPos;
    vec4 fragColor;
    vec2 fragUV;
    vec3 fragNormal;
    vec3 fragTangent;
    vec3 viewPos;
}fs_in;
//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
DECLARE_LIGHT_BUFFER(2, 0, SceneLightsData)
//--------------------------------------------------------
layout(set = 3, binding = 0) uniform sampler2D ShadowMap;
layout(set = 3, binding = 1) uniform ShadowData {
    mat4 matrix;
}LightSpaceMatrix;
//--------------------------------------------------------
// IBL maps
layout(set = 4, binding = 0) uniform samplerCube IrradianceMap;
layout(set = 4, binding = 1) uniform samplerCube PreFilteredMap;
layout(set = 4, binding = 2) uniform sampler2D BrdfLUT;
//------------------------------------------------------------------------------
//layout (set = SET_IDX_USER_DATA_SLOT_0, binding = 1) uniform PBRPassTextures
//{
//    uint ShadowMapIdx;
//    uint IrradianceMapIdx;
//    uint prefilteredMapIdx;
//    uint brdfLUTIdx;
//}texs;
//------------------------------------------------------------------------------
// Output from Fragment Shader : Final Render targets
layout(location = 0) out vec4 outSceneColor;
//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
void main()
{
    vec3 N_Surface = normalize(fs_in.fragNormal);
    vec3 N = normalize(Mat_getNormalMapNormals(fs_in.fragUV, fs_in.fragPos, N_Surface));
    vec3 V = normalize(fs_in.viewPos - fs_in.fragPos);
    vec3 R = reflect(-V, N);

    vec3 albedo = Mat_getAlbedoColor(fs_in.fragUV);
    float metallic = Mat_getMetallicColor(fs_in.fragUV);
    float roughness = Mat_getRoughnessColor(fs_in.fragUV);
    float ao = Mat_getAOColor(fs_in.fragUV);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    for(int i = 0; i < SceneLightsData.numLights; ++i)
    {
        LightData light = SceneLightsData.data[i];

        vec3 L = vec3(0.0f);
        float attenuation = 0.0f;

        if(light.type == LightType_Directional) {
            L = normalize(light.position.xyz);
            attenuation = 1.0f;
        }
        else if(light.type == LightType_Point) {
            L = normalize(light.position - fs_in.fragPos);
            float distance    = length(light.position - fs_in.fragPos);
            attenuation = 1.0 / (distance * distance);
        }

        Lo += CalculateRadiance(L, V, N, F0, albedo, metallic, roughness, light.color, attenuation);
    }

    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    //vec3 irradiance = texture(global_textures_cubemap[nonuniformEXT(texs.IrradianceMapIdx)], N).rgb;
    //vec3 diffuse = irradiance * albedo;
    //
    //const float MAX_REFLECTION_LOD = 4.0;
    //vec3 prefilteredColor = textureLod(global_textures_cubemap[nonuniformEXT(texs.prefilteredMapIdx)], R,  roughness * MAX_REFLECTION_LOD).rgb;
    //vec2 envBRDF  = texture(global_textures_2d[nonuniformEXT(texs.brdfLUTIdx)], vec2(max(dot(N, V), 0.0), roughness)).rg;
    //vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    vec3 irradiance = texture(IrradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 5.0;
    vec3 prefilteredColor = textureLod(PreFilteredMap, R,  roughness * MAX_REFLECTION_LOD).rgb;
    vec2 envBRDF  = texture(BrdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    vec3 ambient = (kD * diffuse + specular ) * ao;

    vec3 result = ambient + Lo;

    //-----------------------------------------------
    // Shadow map calculation
    vec4 FragPosLightSpace = LightSpaceMatrix.matrix * vec4(fs_in.fragPos, 1.0);
    float shadow = 1.0f;
    // FIXME: We assume the first light is the Directional Light and only use that
    if(SceneLightsData.data[0].type == LightType_Directional)
        //shadow = DirectionalShadowCalculation(global_textures_2d[nonuniformEXT(texs.ShadowMapIdx)], FragPosLightSpace, N, SceneLightsData.data[0].position);
        shadow = DirectionalShadowCalculation(ShadowMap, FragPosLightSpace, N, SceneLightsData.data[0].position);

    result *= shadow;
    //-----------------------------------------------

    // Opacity Discard
    if(Mat_getOpacity(fs_in.fragUV) < 0.1)
        discard;

    outSceneColor = vec4(result, Mat_getOpacity(fs_in.fragUV));
    //outSceneColor = vec4(RandomColorHash(gl_PrimitiveID), Mat_getOpacity(fs_in.fragUV));
}

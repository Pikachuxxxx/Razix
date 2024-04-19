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
layout(set = 3, binding = 1) uniform shadowData{
    mat4 LightSpaceMatrix;
}ShadowMapData;
//------------------------------------------------------------------------------
// Output from Fragment Shader : Final Render targets 
layout(location = 0) out vec4 outSceneColor;
//------------------------------------------------------------------------------
void main()
{
    vec3 N = normalize(fs_in.fragNormal);
    vec3 V = normalize(fs_in.viewPos - fs_in.fragPos);

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

        Lo += CalculateRadiance(L, V, N, F0, albedo, metallic, roughness, light.color * light.intensity, attenuation);
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    
    vec3 result = ambient + Lo;

    //-----------------------------------------------
    // Shadow map calculation
    vec4 FragPosLightSpace = ShadowMapData.LightSpaceMatrix * vec4(fs_in.fragPos, 1.0);
    float shadow = 1.0f;
    // FIXME: We assume the first light is the Directiona Light and only use that
    if(SceneLightsData.data[0].type == LightType_Directional)
        shadow = DirectionalShadowCalculation(ShadowMap, FragPosLightSpace, N, SceneLightsData.data[0].position, 0.0f);

    result *= shadow;
    //-----------------------------------------------

    // gamma correct
    //result = pow(result, vec3(1.0/2.2)); 

    outSceneColor = vec4(result, Mat_getOpacity(fs_in.fragUV));
}
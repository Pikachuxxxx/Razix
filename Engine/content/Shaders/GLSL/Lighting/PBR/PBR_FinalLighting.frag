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
#include <Material/Material.glsl>
//-------------------------------
// Lights Data
#include <Lighting/Light.glsl>
//-------------------------------
// PBR - BRDF helper functions
#include <Lighting/PBR/BRDF.glsl>
#include <Lighting/PBR/PBR_DirectLighting.glsl>
//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{
    vec3 fragPos;
    vec4 fragColor;
    vec2 fragTexCoord;
    vec3 fragNormal;
    vec3 fragTangent;
    vec3 viewPos;
}fs_in;
//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
DECLARE_LIGHT_BUFFER(2, 0, sceneLights)
//------------------------------------------------------------------------------
// Output from Fragment Shader : Final Render targets 
layout(location = 0) out vec4 outSceneColor;
//------------------------------------------------------------------------------
void main()
{
    vec3 N = normalize(fs_in.fragNormal);
    vec3 V = normalize(fs_in.viewPos - fs_in.fragPos);

    vec3 albedo = Mat_getAlbedoColor(fs_in.fragTexCoord);
    float metallic = Mat_getMetallicColor(fs_in.fragTexCoord);
    float roughness = Mat_getRoughnessColor(fs_in.fragTexCoord);
    float ao = Mat_getAOColor(fs_in.fragTexCoord);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    for(int i = 0; i < sceneLights.numLights; ++i)
    {
        LightData light = sceneLights.data[i];

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

    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;
    
    vec3 result = ambient + Lo;
    // gamma correct
    result = pow(result, vec3(1.0/2.2)); 

    outSceneColor = vec4(result, getOpacity(fs_in.fragTexCoord));
}
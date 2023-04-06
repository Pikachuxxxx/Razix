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
//--------------------------------------------------------
layout(set = 3, binding = 0) uniform sampler2D shadowMap;
layout(set = 3, binding = 1) uniform ShadowMapData {
    mat4 lightSpaceMatrix;
}shadowMapData;
//--------------------------------------------------------
// IBL maps
layout(set = 3, binding = 2) uniform samplerCube irradianceMap;
layout(set = 3, binding = 3) uniform samplerCube prefilteredMap;
layout(set = 3, binding = 4) uniform sampler2D brdfLUT;
//------------------------------------------------------------------------------
// Output from Fragment Shader : Final Render targets 
layout(location = 0) out vec4 outSceneColor;
//------------------------------------------------------------------------------
// Simple Shadow Map calculation
float DirectionalShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    vec3 transformed_projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, transformed_projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
    float shadow = currentDepth - bias > closestDepth  ? 0.1 : 1.0;  

    return shadow;
}
//------------------------------------------------------------------------------
void main()
{
    vec3 N = normalize(fs_in.fragNormal);
    vec3 V = normalize(fs_in.viewPos - fs_in.fragPos);
    vec3 R = reflect(-V, N); 

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

    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilteredMap, R,  roughness * MAX_REFLECTION_LOD).rgb;   
    vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    vec3 ambient = (specular) * ao; 

    vec3 result = ambient + Lo;

    //-----------------------------------------------
    // Shadow map calculation
    vec4 FragPosLightSpace = shadowMapData.lightSpaceMatrix * vec4(fs_in.fragPos, 1.0);
    float shadow = 1.0f;
    // FIXME: We assume the first light is the Directional Light and only use that
    if(sceneLights.data[0].type == LightType_Directional)
        shadow = DirectionalShadowCalculation(FragPosLightSpace, N, sceneLights.data[0].position);

    result *= shadow;
    //-----------------------------------------------

    // gamma correct
    result = pow(result, vec3(1.0/2.2)); 

    outSceneColor = vec4(result, getOpacity(fs_in.fragTexCoord));
}
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
//#include <Material/Material.glsl>
//-------------------------------
// Lights Data
#include <Lighting/Light.glsl>
//-------------------------------
// PBR - BRDF helper functions
#include <Lighting/PBR/BRDF.glsl>
#include <Lighting/PBR/PBR_DirectLighting.glsl>
#include <Lighting/ShaderInclude.Builtin.ComputeShadows.glsl>
//------------------------------------------------------------------------------
// Vertex Input
layout(location = 0) in VSOutput
{
    vec2 fragUV;
}fs_in;
//--------------------------------------------------------
// Push constant
layout (push_constant) uniform PushConstantData{
    vec3 viewPos;
}pc_data;
//--------------------------------------------------------
// GBuffer Data @ set = 0
layout (set = 0, binding = 0) uniform sampler2D gBuffer0; // .rgb = Normal .a = Position.x
layout (set = 0, binding = 1) uniform sampler2D gBuffer1; // .rgb = Albedo .a = Position.y
layout (set = 0, binding = 2) uniform sampler2D gBuffer2; // .rgb = Emissive .a = Position.z
layout (set = 0, binding = 3) uniform sampler2D gBuffer3; // .r = Metallic .g = roughness .b = AO .a = alpha/opacity
//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
DECLARE_LIGHT_BUFFER(1, 0, sceneLights)
//--------------------------------------------------------
layout(set = 2, binding = 0) uniform sampler2D shadowMap;
layout(set = 2, binding = 1) uniform ShadowMapData {
    mat4 lightSpaceMatrix;
}shadowMapData;
//--------------------------------------------------------
// IBL maps @ set = 2
layout(set = 2, binding = 2) uniform samplerCube irradianceMap;
layout(set = 2, binding = 3) uniform samplerCube prefilteredMap;
layout(set = 2, binding = 4) uniform sampler2D brdfLUT;
//------------------------------------------------------------------------------
// Output from Fragment Shader : Final Render targets 
layout(location = 0) out vec4 outSceneColor;
//------------------------------------------------------------------------------
void main()
{
    vec2 uv = fs_in.fragUV;
    
    vec3 N = normalize(texture(gBuffer0, uv).rgb);
    vec3 Position = texture(gBuffer2, uv).rgb;// vec3(texture(gBuffer0, uv).a, texture(gBuffer1, uv).a, texture(gBuffer2, uv).a);

    vec3 V = normalize(pc_data.viewPos - Position);
    vec3 R = reflect(-V, N); 

    vec3 albedo = texture(gBuffer1, uv).rgb;
    float metallic = texture(gBuffer3, uv).r;
    float roughness = texture(gBuffer3, uv).g;
    float ao = texture(gBuffer3, uv).b;

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
            L = normalize(light.position - Position);
            float distance    = length(light.position - Position);
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
     
    vec3 ambient = (kD * diffuse + specular ) * ao; 

    vec3 result = ambient + Lo;

    //-----------------------------------------------
    // Shadow map calculation
    vec4 FragPosLightSpace = shadowMapData.lightSpaceMatrix * vec4(Position, 1.0);
    float shadow = 1.0f;
    // FIXME: We assume the first light is the Directional Light and only use that
    if(sceneLights.data[0].type == LightType_Directional)
        shadow = DirectionalShadowCalculation(shadowMap, FragPosLightSpace, N, sceneLights.data[0].position);

    result *= shadow;
    //-----------------------------------------------

    // gamma correct
    result = pow(result, vec3(1.0/2.2)); 

    outSceneColor = vec4(result, texture(gBuffer3, uv).a);
}
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
         
         /*
layout(set = 1, binding = 8) uniform sampler2DArrayShadow CascadedShadowMaps;
             
layout(set = 1, binding = 9) uniform sampler3D AccumulatedSH_R;
layout(set = 1, binding = 10) uniform sampler3D AccumulatedSH_G;
layout(set = 1, binding = 11) uniform sampler3D AccumulatedSH_B;

// Grid info for tiled/GI lighting calculation

layout (set = 1, binding = 12) uniform TileData
{
    vec3 MinCorner;
    float _pad;
    vec3 GridSize;
    float CellSize;
} tileData;
*/

//------------------------------------------------------------------------------ 
// Includes
#include <Lighting/Light.glsl>
#include <Lighting/PBR/IBL_AmbientLighting.glsl>
#include <Lighting/PBR/PBR_DirectLighting.glsl>

#include <Utils/Depth.glsl>
#include <Utils/Texture.glsl>

//#include <Lighting/CSM.glsl>
//#include <Lighting/GI/LPV.glsl>
//------------------------------------------------------------------------------ 
//DECLARE_LIGHT_BUFFER(2, 1, g_LightBuffer)
  layout(set = 2, binding = 1, std140) uniform LightBuffer {                     
    uint numLights;                                                                   
    uint _pad[3];                                                                     
    LightData data;                                                                   
  } g_LightBuffer;
//------------------------------------------------------------------------------
// Functions
/*
vec3 CalculateIndirectLight(vec3 fragPos, vec3 N, vec3 albedo, float ao)
{
    const vec3 cellCoords = (fragPos - tileData.MinCorner) / tileData.CellSize / tileData.GridSize;

    const SHcoeffs coeffs = {
        texture(AccumulatedSH_R, cellCoords, 0),
        texture(AccumulatedSH_G, cellCoords, 0),
        texture(AccumulatedSH_B, cellCoords, 0)
    };

    const vec4 SH_intensity = SH_evaluate(-N);
    const vec3 LPV_intensity = vec3(dot(SH_intensity, coeffs.red), dot(SH_intensity, coeffs.green),
           dot(SH_intensity, coeffs.blue));

    vec3 LPV_radiance = max(LPV_intensity * 4 / tileData.CellSize / tileData.CellSize, 0.0);

    vec3 indirectDiffuse = albedo * LPV_radiance * ao;
    return indirectDiffuse;
}
*/
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

    // Dielectrics: [0.02..0.05], Metals: [0.5..1.0]
    const float kMinRoughness = 0.04;
    vec3 F0 = vec3(kMinRoughness);
    const vec3 diffuseColor = mix(albedoColor * (1.0 - F0), vec3(0.0), metallic);
    F0 = mix(F0, albedoColor, metallic);

    const float alphaRoughness = roughness * roughness;

    // Indirect Lighting
    //---------------------------------------------------------------------------
    vec3 Lo_diffuse = vec3(0.0);  // Total Diffuse Lighting
    vec3 Lo_specular = vec3(0.0); // Total Specular Lighting

    // TODO: Check the Render Features to do IBL (AO contribution + reflections)

    // Global Illumination (Diffuse Only)
    //Lo_diffuse += CalculateIndirectLight(fragPosWorldSpace, N, albedoColor, ao);

    // Direct Tiled Lighting (FIXME: Fix tiled once point lights + tiled culling is properly implemented)
    //---------------------------------------------------------------------------
    for (uint i = 0; i < g_LightBuffer.numLights; ++i) {
        const uint lightIndex = i;

        const LightData light = g_LightBuffer.data;//data[lightIndex];

        const vec3 fragToLight = light.type != LightType_Directional
                               ? light.pointLightData.position.xyz - fragPosWorldSpace
                               : -light.dirLightData.direction.xyz;

        const vec3 L = normalize(fragToLight);
        const vec3 H = normalize(V + L);

        const float NdotL = clamp01(dot(N, L));
        if (NdotL > 0.0 || NdotV > 0.0) {
            float visibility = 1.0;
            //if (light.type == LightType_Directional) {
            //    const uint cascadeIndex = _selectCascadeIndex(fragPosViewSpace);
            //    visibility = _getDirLightVisibility(cascadeIndex, fragPosWorldSpace, NdotL);
            //}
            if (visibility == 0.0) continue;

            const vec3 radiance = _getLightIntensity(light, fragToLight) * NdotL * visibility;

            const LightContribution directLighting = PBR_DirectLighting(
                radiance,
                diffuseColor,
                F0,
                specularWeight,
                alphaRoughness,
                NdotV,
                NdotL,
                clamp01(dot(N, H)),
                clamp01(dot(V, H))
            );

            Lo_diffuse += directLighting.diffuse;
        }
    }
    outFragColor = vec4(Lo_diffuse, 1.0f);
    
    outFragColor = vec4(albedoColor, 1.0f);
}
//------------------------------------------------------------------------------  
#ifndef _MATERIAL_HLSL_
#define _MATERIAL_HLSL_

//----------------------------------------------------------------------------
// Note:- When using a combined MetallicRoughnessAO map
// .r = empty .g = Roughness .b  = Metalness .a = AO
//----------------------------------------------------------------------------

// Defined and Constants
enum WorkFlow : uint
{
    WORKFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED,    // In the order of BGR components! AO = r, Roughness = g, Metal = b
    WORKFLOW_PBR_METAL_ROUGHNESS_AO_SEPARATE,
    WORKFLOW_PBR_SPECULAR_GLOSS_COMBINED,
    WORKFLOW_PBR_SPECULAR_GLOSS_SEPARATE,
    WORKFLOW_UNLIT,
    WORKFLOW_LIT_PHONG
};

//----------------------------------------------------------------------------
// Material Data
cbuffer MaterialData : register(b0, space1)
{
    float3 baseColor;
    float3 normal;
    float  emissiveIntensity;
    float  metallic;
    float  roughness;
    float  specular;
    float  opacity;
    float  ambientOcclusion;
    float2 uvScale;
    bool   visible;
    uint   workflow;

    bool isUsingAlbedoMap;
    bool isUsingNormalMap;
    bool isUsingMetallicMap;
    bool isUsingRoughnessMap;
    bool isUsingSpecular;
    bool isUsingEmissiveMap;
    bool isUsingAOMap;

    uint AlbedoMapIdx;
    uint NormalMapIdx;
    uint MetallicMapIdx;
    uint RoughnessMapIdx;
    uint SpecularMapIdx;
    uint EmissiveMapIdx;
    uint AOMapIdx;
}

//----------------------------------------------------------------------------
// Material Textures
namespace MaterialTextures {
    Texture2D albedoMap : register(t1, space1);
    Texture2D normalMap : register(t2, space1);
    Texture2D metallicMap : register(t3, space1);    // Can also be combined MetallicRoughnessAO
    Texture2D roughnessMap : register(t4, space1);
    Texture2D specularMap : register(t5, space1);
    Texture2D emissiveMap : register(t6, space1);
    Texture2D aoMap : register(t7, space1);

    SamplerState linearSampler : register(s0, space1);
}    // namespace MaterialTextures
//----------------------------------------------------------------------------
// Helper Functions
float3 Mat_getAlbedoColor(float2 uv)
{
    uv *= uvScale;
    if (isUsingAlbedoMap)
        return MaterialTextures::albedoMap.Sample(MaterialTextures::linearSampler, uv).rgb;
    else
        return baseColor * emissiveIntensity;
}

float3 Mat_getNormalMapNormals(float2 uv, float3 worldPos, float3 N)
{
    uv *= uvScale;
    if (isUsingNormalMap) {
        float3 tangentNormal = MaterialTextures::normalMap.Sample(MaterialTextures::linearSampler, uv).xyz * 2.0 - 1.0;

        float3 Q1  = ddx(worldPos);
        float3 Q2  = ddy(worldPos);
        float2 st1 = ddx(uv);
        float2 st2 = ddy(uv);

        float3   T   = normalize(Q1 * st2.y - Q2 * st1.y);
        float3   B   = normalize(cross(N, T));
        float3x3 TBN = float3x3(T, B, N);

        return normalize(mul(tangentNormal, TBN));
    }
    return N;
}

float Mat_getMetallicColor(float2 uv)
{
    uv *= uvScale;
    if (isUsingMetallicMap) {
        if (workflow == WorkFlow::WORKFLOW_PBR_METAL_ROUGHNESS_AO_SEPARATE)
            return MaterialTextures::metallicMap.Sample(MaterialTextures::linearSampler, uv).r;
        else if (workflow == WorkFlow::WORKFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED)
            return MaterialTextures::metallicMap.Sample(MaterialTextures::linearSampler, uv).b;
    }
    return metallic;
}

float Mat_getRoughnessColor(float2 uv)
{
    uv *= uvScale;
    if (isUsingRoughnessMap) {
        if (workflow == WorkFlow::WORKFLOW_PBR_METAL_ROUGHNESS_AO_SEPARATE)
            return MaterialTextures::roughnessMap.Sample(MaterialTextures::linearSampler, uv).r;
        else if (workflow == WorkFlow::WORKFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED)
            return MaterialTextures::metallicMap.Sample(MaterialTextures::linearSampler, uv).g;
    }
    return roughness;
}

float3 getSpecularColor(float2 uv)
{
    uv *= uvScale;
    if (isUsingSpecular)
        return MaterialTextures::specularMap.Sample(MaterialTextures::linearSampler, uv).rgb;
    return float3(1.0f, 1.0f, 1.0f);
}

float Mat_getAOColor(float2 uv)
{
    uv *= uvScale;
    if (isUsingAOMap && workflow == WorkFlow::WORKFLOW_PBR_METAL_ROUGHNESS_AO_SEPARATE)
        return MaterialTextures::aoMap.Sample(MaterialTextures::linearSampler, uv).r;
    else if (isUsingMetallicMap && workflow == WorkFlow::WORKFLOW_PBR_METAL_ROUGHNESS_AO_COMBINED)
        return MaterialTextures::metallicMap.Sample(MaterialTextures::linearSampler, uv).r;
    return 1.0f;
}

float Mat_getOpacity(float2 uv)
{
    uv *= uvScale;
    if (isUsingAlbedoMap)
        return MaterialTextures::albedoMap.Sample(MaterialTextures::linearSampler, uv).a;
    return opacity;
}

#endif

/*
 * Razix Engine HLSL Pixel Shader File
 * Calculate the Final PBR direct lighting 
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
// Lights Data
#include <Lighting/ShaderInclude.Builtin.Light.h>
//-------------------------------
// PBR - BRDF helper functions
#include <Lighting/PBR/ShaderInclude.Builtin.DisneyBRDF.h>
//-------------------------------
// Color Utils (Debug only)
#include <Utils/ShaderInclude.Builtin.Color.h>
//------------------------------------------------------------------------------
// Vertex Input
struct VSOutput
{
    float2 UV : TEXCOORD0;
};
//--------------------------------------------------------
// Push constants
struct PushConstantData
{
    float3 camViewPos;
    float dt;
    float4x4 viewMatrix;
};
PUSH_CONSTANT(PushConstantData);
//------------------------------------------------------------------------------
// @ slot #0 - .rgb = Normal   .a = Metallic
// @ slot #1 - .rgb = Albedo   .a = Roughness
// @ slot #2 - .rgb = Position .a = AO
Texture2D gBuffer0 : register(t0, space1); 
Texture2D gBuffer1 : register(t1, space1); 
Texture2D gBuffer2 : register(t2, space1); 
SamplerState linearSampler : register(s3, space1);
//------------------------------------------
Texture2D ShadowMap : register(t0, space2); 
SamplerState shadowSampler : register(s1, space2);
cbuffer ShadowData : register(b2, space2)
{
    float4x4 LightSpaceMatrix;
};
//------------------------------------------
// IBL maps
TextureCube IrradianceMap  : register(t0, space3);
TextureCube PreFilteredMap : register(t1, space3);
Texture2D BrdfLUT : register(t2, space3);
SamplerState cubeSampler : register(s3, space3);
//------------------------------------------------------------------------------
// Final Render targets 
struct PSOut
{
    float4 SceneColor: SV_Target0;
};
//------------------------------------------------------------------------------
PSOut PS_MAIN(VSOutput input)
{
    PSOut output;
    output.SceneColor = float4(1.0f, 0.0f, 1.0f, 1.0f);

    return output;
}
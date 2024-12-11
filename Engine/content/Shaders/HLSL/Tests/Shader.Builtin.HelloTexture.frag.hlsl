/*
 * Razix Engine HLSL Pixel Shader File
 * Pixel Shader to test texture mapping in razix engine, maps a textures onto a Hello Triangle
 */
//------------------------------------------------------------------------------
#include "../../ShaderCommon/ShaderInclude.Builtin.ShaderLangCommon.h"
//------------------------------------------------------------------------------
struct PSIn
{
    float4 Position : SV_POSITION;
    float4 Color    : COLOR0;
    float2 UV       : TEXCOORD0;
};

Texture2D g_TestTexture : register(t0, space0);
SamplerState g_Sampler : register(s0, space0);

 //------------------------------------------------------------------------------
float4 PS_MAIN(PSIn psIn) : SV_TARGET
{
    return psIn.Color * g_TestTexture.Sample(g_Sampler, psIn.UV);
}
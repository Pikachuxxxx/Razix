/*
 * Razix Engine HLSL Pixel Shader File
 * Pixel Shader to test texture mapping in razix engine, maps a textures onto a Hello Triangle
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
struct VSInput
{
    float4 Position : SV_POSITION;
    float4 Color    : COLOR0;
    float2 UV       : TEXCOORD0;
};
//------------------------------------------------------------------------------
SamplerState g_Sampler : register(s0, space0);
Texture2D g_TestTexture : register(t0, space1);
//------------------------------------------------------------------------------
float4 PS_MAIN(VSInput psIn) : SV_TARGET
{
    return psIn.Color * g_TestTexture.SampleLevel(g_Sampler, psIn.UV, 0);
}

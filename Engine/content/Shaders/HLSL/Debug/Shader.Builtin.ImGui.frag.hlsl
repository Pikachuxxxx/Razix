/*
 * Razix Engine HLSL Pixel Shader File
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
struct VSInput
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR0;
};
//------------------------------------------------------------------------------
SamplerState g_FontSampler : register(s0, space0);
Texture2D    g_FontTexture : register(t0, space1);
//------------------------------------------------------------------------------
float4 PS_MAIN(VSInput input)
    : SV_TARGET
{
    return input.Color * g_FontTexture.Sample(g_FontSampler, input.UV);
}
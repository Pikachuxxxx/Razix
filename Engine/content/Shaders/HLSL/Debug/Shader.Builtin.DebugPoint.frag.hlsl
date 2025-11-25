/*
 * Razix Engine HLSL Pixel Shader File
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
struct PSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
    float2 UV : TEXCOORD0;
};

//------------------------------------------------------------------------------

float LinearizeDepth(float depth)
{
    float n = 0.1;      // TODO: pass camera z near from PSInput
    float f = 100.0;    // TODO: pass camera z far from PSInput
    float z = depth;
    return (2.0 * n) / (f + n - z * (f - n));
}

float4 PS_MAIN(PSInput input)
    : SV_TARGET
{
    float distSq = dot(input.UV, input.UV);
    if (distSq > 1.0) {
        discard;
    }
    return input.Color;
}
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
};
//------------------------------------------------------------------------------

float4 PS_MAIN(PSInput input)
    : SV_TARGET
{
    return input.Color;
}
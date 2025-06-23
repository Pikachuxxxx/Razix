/*
 * Razix Engine HLSL Pixel Shader File
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
struct VSInput
{
    float4 Color : COLOR;
};
//------------------------------------------------------------------------------

float4 PS_MAIN(VSInput input)
    : SV_TARGET
{
    return input.Color;
}
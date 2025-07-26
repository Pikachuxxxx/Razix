/*
 * Razix Engine HLSL Pixel Shader File
 * Test Pixel Shader used to render a Hello Triangle
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------

struct PSIn
{
    float4 Position : SV_POSITION;
    float4 Color    : COLOR0;
    float2 UV       : TEXCOORD0;
};
 //------------------------------------------------------------------------------
float4 PS_MAIN(PSIn psIn) : SV_TARGET
{
    return psIn.Color;// float4(psIn.UV, 0.00f, 1.0f);
}
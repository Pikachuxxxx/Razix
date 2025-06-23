/*
 * Razix Engine HLSL Vertex Shader File
 * Debug shader used to draw lines in the world
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
// Vertex Input
struct VertexInput
{
    float2 Position : POSITION0;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR0;
};
//------------------------------------------------------------------------------
struct PushConstant
{
    float2 scale;
    float2 translate;
};
PUSH_CONSTANT(PushConstant);
//------------------------------------------------------------------------------
struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR0;
};
//------------------------------------------------------------------------------
VSOutput VS_MAIN(VertexInput input)
{
    VSOutput output;

    output.Position = float4(input.Position * GET_PUSH_CONSTANT(scale) + GET_PUSH_CONSTANT(translate), 0.0, 1.0);
    output.UV       = input.UV;
    output.Color    = input.Color;
    return output;
}
//------------------------------------------------------------------------------

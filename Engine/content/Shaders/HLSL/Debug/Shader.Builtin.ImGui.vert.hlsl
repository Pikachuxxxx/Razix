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
    float2 Position: POSITION;
    float2 UV: TEXCOORD;
    float4 Color: COLOR;
};
//------------------------------------------------------------------------------
struct PushConstant {
	float2 scale;
	float2 translate;
};
PUSH_CONSTANT(PushConstant);
//------------------------------------------------------------------------------
struct VSOutput
{
    float4 Position: SV_POSITION;
    float2 UV: TEXCOORD;
    float4 Color: COLOR;
};
//------------------------------------------------------------------------------
VSOutput VS_MAIN(VertexInput input)
{
    VSOutput output;

    output.Position = float4(mul(input.Position, GET_PUSH_CONSTANT(scale)) + GET_PUSH_CONSTANT(translate), 0.0, 1.0);
    output.UV = input.UV;
    output.Color = input.Color;
    return output;
}
//------------------------------------------------------------------------------

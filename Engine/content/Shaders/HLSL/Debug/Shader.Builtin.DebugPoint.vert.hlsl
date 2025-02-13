/*
 * Razix Engine HLSL Vertex Shader File
 * Debug shader used to draw points in the world
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
// Includes
#include <Common/ShaderInclude.Builtin.FrameData.h>
//------------------------------------------------------------------------------
// Vertex Input
struct VertexInput
{
    float4 Position: POSITION;
    float4 Color: COLOR;
    float2 Size:TEXCOORD0;
    float2 UV: TEXCOORD1;
};
//------------------------------------------------------------------------------
struct VSOutput
{
    float4 Position: SV_POSITION;
    float4 Color: COLOR;
    float2 Size:TEXCOORD0;
    float2 UV: TEXCOORD1;
};
//------------------------------------------------------------------------------
VSOutput VS_MAIN(VertexInput input)
{
    VSOutput output;

    output.Position = mul(frame_info.camera.projection, mul(frame_info.camera.view, input.Position));
    output.Color = input.Color;
    output.Size = input.Size;
    output.UV = input.UV;

    return output;
}
//------------------------------------------------------------------------------

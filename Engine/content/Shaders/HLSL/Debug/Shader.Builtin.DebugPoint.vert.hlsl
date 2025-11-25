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
    float4 Position : POSITION;
    float4 Color : COLOR;
};
//------------------------------------------------------------------------------
struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
    float2 UV : TEXCOORD0;
};

static const float2 UVs[4] = {
    float2(-1.0, 1.0),
    float2(1.0, -1.0),
    float2(1.0, 1.0),
    float2(-1.0, 1.0)};

//------------------------------------------------------------------------------
VSOutput VS_MAIN(VertexInput input, uint vertexID : SV_VertexID)
{
    VSOutput output;

    output.Position = mul(frame_info.camera.projection, mul(frame_info.camera.view, input.Position));
    output.Color    = input.Color;
    output.UV       = UVs[vertexID % 4];    // Assign UVs based on vertex index

    return output;
}
//------------------------------------------------------------------------------

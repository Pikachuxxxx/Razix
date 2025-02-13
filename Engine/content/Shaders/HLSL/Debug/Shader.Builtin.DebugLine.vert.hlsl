/*
 * Razix Engine HLSL Vertex Shader File
 * Debug shader used to draw lines in the world
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
    float4 Color: Color;
};
//------------------------------------------------------------------------------
struct VSOutput
{
    float4 Position: SV_POSITION;
    float4 Color: Color;
};
//------------------------------------------------------------------------------
VSOutput VS_MAIN(VertexInput input)
{
    VSOutput output;

    output.Position = mul(frame_info.camera.projection, mul(frame_info.camera.view, input.Position));
    output.Color = input.Color;
    return output;
}
//------------------------------------------------------------------------------

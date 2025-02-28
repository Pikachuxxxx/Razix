/*
 * Razix Engine HLSL Vertex Shader File
 * Default Vertex Shader that can be used for rendering basic geometry
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
// Includes
#include <Common/ShaderInclude.Builtin.FrameData.h>
#include <Common/ShaderInclude.Builtin.VertexInput.h>
//------------------------------------------------------------------------------
// Buffers and Root Constants
// The model push constant
struct PushConstant 
{
    float4x4 worldTransform;
    float4x4 previousWorldTransform;
};
PUSH_CONSTANT(PushConstant);
//------------------------------------------------------------------------------
struct VSOut
{
    float4 Position   : SV_POSITION;
    float4 WorldPos   : POSITION0;
    float4 Color      : COLOR;
    float2 UV         : TEXCOORD;
    float3 Normal     : NORMAL;
    float3 Tangent    : TANGENT;
};
//------------------------------------------------------------------------------
VSOut VS_MAIN(VSIn vsIn)
{
    VSOut vso;

    float4 transformedPos = mul(GET_PUSH_CONSTANT(worldTransform), float4(vsIn.inPosition, 1.0f));
    vso.WorldPos = transformedPos;

    transformedPos = mul(frame_info.camera.view, transformedPos);
    transformedPos = mul(frame_info.camera.projection, transformedPos);
    vso.Position = transformedPos;
    vso.Color = vsIn.inColor;
    vso.UV = vsIn.inTexCoord;
    vso.Normal = vsIn.inNormal; // this should be normal matrix scaled
    vso.Tangent = vsIn.inTangent;

    return vso;
}

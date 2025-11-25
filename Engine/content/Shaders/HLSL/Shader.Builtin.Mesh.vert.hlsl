/*
 * Razix Engine HLSL Vertex Shader File
 * Default Vertex Shader that can be used for rendering basic geometry
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
// Includes
#include <Common/ShaderInclude.Builtin.FrameData.h>
#include <Razix/Shared/RZShaderCompitability.h>
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
    float4 Position : SV_POSITION;
    float4 WorldPos : POSITION0;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};
//------------------------------------------------------------------------------
VSOut VS_MAIN(VSIn vsIn)
{
    VSOut vso;

    float4 transformedPos = mul(GET_PUSH_CONSTANT(worldTransform), float4(vsIn.Position, 1.0f));
    vso.WorldPos          = transformedPos;

    transformedPos = mul(frame_info.camera.view, transformedPos);
    transformedPos = mul(frame_info.camera.projection, transformedPos);
    vso.Position   = transformedPos;
    vso.Color      = vsIn.Color;
    vso.UV         = vsIn.UV;
    vso.Normal     = vsIn.Normal;    // this should be normal matrix scaled
    vso.Tangent    = vsIn.Tangent;

    return vso;
}

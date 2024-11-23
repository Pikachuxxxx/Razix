/*
 * Razix Engine HLSL Vertex Shader File
 * Default Vertex Shader that can be used for rendering basic geometry
 */
//------------------------------------------------------------------------------
#include "../ShaderCommon/ShaderInclude.Builtin.ShaderLangCommon.h"
//------------------------------------------------------------------------------
// Includes
#include "Common/ShaderInclude.Builtin.FrameData.h"
//------------------------------------------------------------------------------

// Vertex Input
struct VSIn
{
    float3 inPosition   : POSITION;
    float4 inColor      : COLOR;
    float2 inTexCoord   : TEXCOORD;
    float3 inNormal     : NORMAL;
};
//------------------------------------------------------------------------------
// Buffers and Root Constants
// The model push constant
// TODO: Make this a root constant
struct ModelPushConstantData 
{
    float4x4 worldTransform;
    float4x4 previousWorldTransform;
};

PUSH_CONSTANT(ModelPushConstantData);

//------------------------------------------------------------------------------
struct VSOut
{
    float4 Position   : SV_POSITION;
    float4 Color      : COLOR;
    float2 UV         : TEXCOORD;
    float3 Normal     : NORMAL;
};
//------------------------------------------------------------------------------
VSOut VS_MAIN(VSIn vsIn)
{
    VSOut vso;

    float4 transformedPos = mul(GET_PUSH_CONSTANT(worldTransform), float4(vsIn.inPosition, 1.0f));
    transformedPos = mul(frame_info.camera.view, transformedPos);
    transformedPos = mul(frame_info.camera.projection, transformedPos);
    vso.Position = transformedPos;

    return vso;
}

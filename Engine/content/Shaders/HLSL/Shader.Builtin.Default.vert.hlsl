/*
 * Razix Engine HLSL Vertex Shader File
 * Default Vertex Shader that can be used for rendering basic geometry with vertex colors and use a texture as well
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
// Vertex Input
struct VSIn
{
    float3 inPosition : POSITION;
    float4 inColor : COLOR;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};
//------------------------------------------------------------------------------
// Buffers and Root Constants
// The view projection matrix
cbuffer ViewProjectionBuffer : register(b0, space0)
{
    matrix view;
    matrix proj;
};
// The model push constant
// TODO: Make this a root constant
cbuffer ModelPushConstantData : register(b1, space0)
{
    float4x4 worldTransform;
    float4x4 previousWorldTransform;
};
//------------------------------------------------------------------------------
struct VSOut
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
};
//------------------------------------------------------------------------------
VSOut VS_MAIN(VSIn vsIn)
{
    VSOut vso;

    float4 transformedPos = mul(worldTransform, float4(vsIn.inPosition, 1.0f));
    transformedPos        = mul(view, transformedPos);
    transformedPos        = mul(proj, transformedPos);
    vso.Position          = transformedPos;
    vso.Color             = vsIn.inColor;
    vso.UV                = vsIn.inTexCoord;
    vso.Normal            = vsIn.inNormal;

    return vso;
}

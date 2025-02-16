/*
 * Razix Engine HLSL Vertex Shader File
 * Vertex shader to render a depth texture onto a Texture2D
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
// Includes
#include <Common/ShaderInclude.Builtin.VertexInput.h>
//------------------------------------------------------------------------------
// Uniforms and Push Constants 
// view projection matrix
cbuffer LightSpaceMatrix: register(b0, space0)
{
    float4x4 lightSpaceMat;
};

struct PushConstant 
{
    float4x4 worldTransform;
    float4x4 previousWorldTransform;
};
PUSH_CONSTANT(PushConstant);
//------------------------------------------------------------------------------
float4 VS_MAIN(VSIn vsInput) : SV_POSITION
{
    float4 transformedPos = mul(GET_PUSH_CONSTANT(worldTransform), float4(vsInput.inPosition, 1.0f));
    transformedPos = mul(lightSpaceMat, transformedPos);

    return transformedPos;
}
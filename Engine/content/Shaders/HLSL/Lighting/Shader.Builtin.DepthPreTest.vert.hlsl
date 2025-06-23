/*
 * Razix Engine HLSL Vertex Shader File
 * Vertex shader to render a depth texture onto a Texture2D
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
#include <Common/ShaderInclude.Builtin.VertexInput.h>
//------------------------------------------------------------------------------
// Uniforms and Push Constants
// view projection matrix
cbuffer LightSpaceMatrix : register(b0, space0)
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
struct VSOut
{
    float4 Position : SV_POSITION;
};

VSOut VS_MAIN(VSIn vsInput)
{
    VSOut output;

    float4 transformedPos = mul(GET_PUSH_CONSTANT(worldTransform), float4(vsInput.inPosition, 1.0f));
    output.Position       = mul(lightSpaceMat, transformedPos);

    return output;
}
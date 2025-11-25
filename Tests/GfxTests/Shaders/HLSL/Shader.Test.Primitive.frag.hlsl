/*
 * Razix Engine HLSL Pixel Shader File
 * Shader.Test.Primitive.frag.hlsl : Visualizes world-space normals as RGB colors
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
struct PSIn
{
    float4 Position : SV_POSITION;
    float3 NormalWS : NORMAL0;
};
//------------------------------------------------------------------------------
float4 PS_MAIN(PSIn input) : SV_TARGET
{
    float3 n = normalize(input.Position.rgb) * 0.5f + 0.5f;
    return float4(n, 1.0f);
}

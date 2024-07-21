/*
 * Razix Engine GLSL Vertex Shader File
 * Default Vertex Shader that can be used for rendering basic geometry with vertex colors and use a texture as well
 */
//------------------------------------------------------------------------------
#include "../ShaderCommon/ShaderInclude.Builtin.ShaderLangCommon.h"
//------------------------------------------------------------------------------

struct VSOut
{
    float4 pos : SV_Position;
    float4 color : Color;
};

cbuffer CBuf : register (b0)
{
    matrix transform;
};

VSOut VS_MAIN( float3 pos : Position, float4 color : Color )
{
    VSOut vso;
    vso.pos = mul(float4(pos, 1.0f), transform);
    vso.color = color;
    return vso;
}

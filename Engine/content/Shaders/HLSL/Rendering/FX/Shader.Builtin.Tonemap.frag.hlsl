/*
 * Razix Engine HLSL Pixel Shader File
 * Applies Tonemapping to the final scene HDR render target
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
#include <Rendering/FX/ShaderInclude.Builtin.TonemappingFuncs.h>
//------------------------------------------------------------------------------
// Vertex Input from Quad.vert
struct VSOutput
{
    float4 position : SV_POSITION;
    float2 UV : TEXCOORD0;
};
//------------------------------------------------------------------------------
// GBuffer Deferred resource bindings
SamplerState g_Sampler : register(s0, space0);
Texture2D    SceneHDRRenderTarget : register(t0, space1);
//------------------------------------------------------------------------------
// Push constants
struct PushConstant
{
    uint tonemapMode;
};
PUSH_CONSTANT(PushConstant);
//------------------------------------------------------------------------------
// Final Render targets
struct PSOut
{
    float4 SceneColor : SV_Target0;
};
//------------------------------------------------------------------------------
PSOut PS_MAIN(VSOutput input)
{
    float2 uv     = float2(input.UV.x, input.UV.y);
    float3 result = SceneHDRRenderTarget.Sample(g_Sampler, uv).rgb;

    // Tonemap
    switch (GET_PUSH_CONSTANT(tonemapMode)) {
        case 0:
            result = TonemapFuncs::ACES(result);
            break;
        case 1:
            result = TonemapFuncs::Filmic(result);
            break;
        case 2:
            result = TonemapFuncs::Lottes(result);
            break;
        case 3:
            result = TonemapFuncs::Reinhard(result);
            break;
        case 4:
            result = TonemapFuncs::Reinhard2(result);
            break;
        case 5:
            result = TonemapFuncs::Uchimura(result);
            break;
        case 6:
            result = TonemapFuncs::Uncharted2(result);
            break;
        case 7:
            result = TonemapFuncs::Unreal(result);
            break;
        case 8:
            result = result;    // No Tonemapping, let UNORM render target normalize it or write as-is, will cause clipping
            break;
    }

    PSOut output;
    output.SceneColor = float4(result, 1.0f);
    return output;
}
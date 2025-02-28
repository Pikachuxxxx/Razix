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
    float2 UV : TEXCOORD0;
};
//------------------------------------------------------------------------------
// GBuffer Deferred resource bindings
Texture2D SceneHDRRenderTarget : register(t0);
SamplerState linearSampler : register(s0);
//------------------------------------------------------------------------------
// Push constants
struct PushConstantData
{
    uint tonemapMode;
};
PUSH_CONSTANT(PushConstantData);
//------------------------------------------------------------------------------
// Final Render targets 
struct PSOut
{
    float4 SceneColor : SV_Target0;
};
//------------------------------------------------------------------------------
PSOut PS_MAIN(VSOutput input)
{
    float2 uv = float2(input.UV.x, input.UV.y);
    float3 result = SceneHDRRenderTarget.Sample(linearSampler, uv).rgb;
    
    // Tonemap
    switch(GET_PUSH_CONSTANT(tonemapMode))
    {
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
            result = result; // No tonemapping
            break;
    }
    
    PSOut output;
    output.SceneColor = float4(result, 1.0f);
    return output;
}
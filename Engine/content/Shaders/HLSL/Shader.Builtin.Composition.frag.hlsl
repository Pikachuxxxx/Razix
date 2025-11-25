/*
 * Razix Engine Shader File
 * Default Pixel Shader composition pass for presentation onto a swapchain image, takes a RT and presents it onto a Screen Quad 
 */
//------------------------------------------------------------------------------
// Bindless Textures
#include <Utils/ShaderInclude.Builtin.Color.h>
//------------------------------------------------------------------------------
// VersampleColor Input
struct PsIn
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD0;
};
//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
SamplerState g_Sampler         : register(s0, space0);
Texture2D    FinalSceneColor   : register(t0, space1);
//------------------------------------------------------------------------------
float4 PS_MAIN(PsIn input)
    : SV_TARGET
{
    float4 result = FinalSceneColor.Sample(g_Sampler, input.uv);
    return result;
}
//------------------------------------------------------------------------------

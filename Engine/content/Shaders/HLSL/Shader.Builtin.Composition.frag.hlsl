/*
 * Razix Engine Shader File
 * Default Pixel Shader composition pass for presention onto a swapchain image, takes a RT and presents it onto a Screen Quad 
 */
//------------------------------------------------------------------------------
// Bindless Textures
//#include <Utils/ShaderInclude.Builtin.Color.glsl>
//------------------------------------------------------------------------------
// VersampleColor Input
struct PsIn
{
    float2 uv : TEXCOORD0;
};
//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
Texture2D CompositionTarget : register(t0, space0);
SamplerState g_ColorSampler : register(s1, space0);
//------------------------------------------------------------------------------
float4 PS_MAIN(PsIn input) : SV_TARGET
{
    float4 result = CompositionTarget.Sample(g_ColorSampler, input.uv);

    // Gamma correction to sRGB
    // result = LinearTosRGB(result);
    return result;
}
//------------------------------------------------------------------------------

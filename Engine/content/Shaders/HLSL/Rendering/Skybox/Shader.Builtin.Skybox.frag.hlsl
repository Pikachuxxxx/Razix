/*
 * Razix Engine Shader File
 * Pixels shader to render a skybox as a cubemap
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Vertex Input
struct PsIn
{
    float4 Position          : SV_POSITION;
    float4 PixelLocalPosTime : POSITION0;
};
//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
SamplerState        g_Sampler       : register(s0, space1);
TextureCube<float4> EnvironmentMap  : register(t0, space2);
//------------------------------------------------------------------------------
float4 PS_MAIN(PsIn input)
    : SV_TARGET
{
    return EnvironmentMap.Sample(g_Sampler, input.PixelLocalPosTime.xyz);
}
//------------------------------------------------------------------------------

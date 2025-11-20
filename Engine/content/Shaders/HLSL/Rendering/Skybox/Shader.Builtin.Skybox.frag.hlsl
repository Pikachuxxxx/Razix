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
TextureCube<float4> EnvironmentMap  : register(t0, space0);
SamplerState        HDRSampler      : register(s0, space1);
//------------------------------------------------------------------------------
float4 PS_MAIN(PsIn input)
    : SV_TARGET
{
    return EnvironmentMap.Sample(HDRSampler, input.PixelLocalPosTime.xyz);
}
//------------------------------------------------------------------------------

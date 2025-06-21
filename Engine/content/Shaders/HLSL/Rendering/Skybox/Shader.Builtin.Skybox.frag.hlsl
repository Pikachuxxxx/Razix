/*
 * Razix Engine Shader File
 * Pixels shader to render a skybox as a cubemap
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Vertex Input
struct PsIn
{
    float4 PixelLocalPosTime : POSITION1;
};
//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
TextureCube<float4> environmentMap : register(t0, space1);
SamplerState        g_Sampler : register(s1, space1);
//------------------------------------------------------------------------------
float4 PS_MAIN(PsIn input)
    : SV_TARGET
{
    return environmentMap.Sample(g_Sampler, input.PixelLocalPosTime.xyz);
}
//------------------------------------------------------------------------------

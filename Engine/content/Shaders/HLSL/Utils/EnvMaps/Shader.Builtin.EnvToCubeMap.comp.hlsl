/*
 * Razix Engine HLSL Compute Shader File
 * Compute shader to convert equi-rectangular hdr file to cube map, write directly to the cubemap
 */
//------------------------------------------------------------------------------
#include <Utils/EnvMaps/ShaderInclude.Builtin.EnvmapUtils.h>
//------------------------------------------------------------------------------
SamplerState             HDRSampler : register(s0, space0);    // Sampler for the HDR texture
Texture2D                HDRTexture : register(t0, space1);    // Equirectangular HDR texture
RWTexture2DArray<float4> CubeMapRT : register(u1, space1);
//------------------------------------------------------------------------------
[numthreads(32, 32, 1)] void CS_MAIN(uint3 DTid : SV_DispatchThreadID) {
    uint   faceIdx    = DTid.z;    // on CPU we have RHI::Dispatch(W/32, H/32, NUM_FACES = 6)
    uint2  localCoord = DTid.xy;
    float2 uv         = float2(localCoord) / float2(CUBEMAP_DIM, CUBEMAP_DIM);

    float3 direction = UVToDirection(uv, faceIdx);
    float2 eqUV      = DirectionToEquirectangularUV(direction);
    float4 hdrColor  = HDRTexture.SampleLevel(HDRSampler, eqUV, 0);

    CubeMapRT[DTid] = hdrColor;
}

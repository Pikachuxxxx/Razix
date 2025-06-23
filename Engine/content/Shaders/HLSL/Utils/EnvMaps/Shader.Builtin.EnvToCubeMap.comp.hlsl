/*
 * Razix Engine HLSL Compute Shader File
 * Compute shader to convert equi-rectangular hdr file to cube map, write directly to the cubemap
 */
//------------------------------------------------------------------------------
#define GEN_ENVMAP    // siomn
#include <Utils/EnvMaps/ShaderInclude.Builtin.EnvmapUtils.h>
//------------------------------------------------------------------------------
Texture2D                HDRTexture : register(t0);    // Equirectangular HDR texture
SamplerState             HDRSampler : register(s1);    // Sampler for the HDR texture
RWTexture2DArray<float4> CubeMapRT : register(u2);
//------------------------------------------------------------------------------
cbuffer Constants : register(b3)
{
    uint2 cubeFaceSize;
    uint  mipLevel;
};
//------------------------------------------------------------------------------
[numthreads(32, 32, 1)] void CS_MAIN(uint3 DTid : SV_DispatchThreadID) {
    uint   faceIdx    = DTid.z;    // on CPU we have RHI::Dispatch(W/32, H/32, NUM_FACES = 6)
    uint2  localCoord = DTid.xy;
    float2 uv         = float2(localCoord) / float2(cubeFaceSize);

    float3 direction = UVToDirection(uv, faceIdx);
    float2 eqUV      = DirectionToEquirectangularUV(direction);
    float4 hdrColor  = HDRTexture.SampleLevel(HDRSampler, eqUV, mipLevel);

    CubeMapRT[DTid] = hdrColor;
}

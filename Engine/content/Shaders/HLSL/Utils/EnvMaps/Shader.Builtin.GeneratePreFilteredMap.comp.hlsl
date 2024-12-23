/*
 * Razix Engine HLSL Compute Shader File
 * Compute shader to convert a CubeMap to Irradiance map
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
#include <Utils/EnvMaps/ShaderInclude.Builtin.EnvmapUtils.hlsl>
//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
TextureCube EnvCubeMap : register(t0);                  // Environment Cubemap
SamplerState EnvCubeSampler : register(s1);             // Sampler for the HDR texture
RWTexture2DArray<float4> PreFitleredMap : register(u2);
//------------------------------------------------------------------------------
struct PushConstant
{
    uint2 cubeFaceSize;
	float roughness;
    uint mipLevel;
};
PUSH_CONSTANT(PushConstant);
//------------------------------------------------------------------------------
// Helper functions

//------------------------------------------------------------------------------
// TODO: Optimization - use SH 
[numthreads(32, 32, 1)]	
void CS_MAIN(uint3 DTid: SV_DispatchThreadID)
{
    uint faceIdx = DTid.z; // on CPU we have RHI::Dispatch(W/32, H/32, NUM_FACES = 6)
    uint2 localCoord = DTid.xy;
    float2 uv = float2(localCoord) / float2(GET_PUSH_CONSTANT(cubeFaceSize));
    float3 N = UVToDirection(uv, faceIdx);

    PreFitleredMap[DTid] = float4(N, 1.0f);
}

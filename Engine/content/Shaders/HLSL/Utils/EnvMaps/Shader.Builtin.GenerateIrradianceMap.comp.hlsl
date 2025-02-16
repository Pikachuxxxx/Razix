/*
 * Razix Engine HLSL Compute Shader File
 * Compute shader to convert a cubemap to Irradiance map
 */
//------------------------------------------------------------------------------
#include <Razix/Shared/RZShaderCompitability.h>
#include <Utils/EnvMaps/ShaderInclude.Builtin.EnvmapUtils.h>
//------------------------------------------------------------------------------
TextureCube EnvCubeMap : register(t0); // Environment Cubemap
SamplerState EnvCubeSampler : register(s1); // Sampler for the HDR texture
RWTexture2DArray<float4> IrradianceMap : register(u2);
//------------------------------------------------------------------------------
cbuffer Constants : register(b3)
{
    uint2 cubeFaceSize;
    uint mipLevel;
};
//------------------------------------------------------------------------------
// TODO: Optimization - use SH 
// [Source]: copied from https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/irmap.hlsl
[numthreads(32, 32, 1)]	
void CS_MAIN(uint3 DTid: SV_DispatchThreadID)
{
    uint faceIdx = DTid.z; // on CPU we have RHI::Dispatch(W/32, H/32, NUM_FACES = 6)
    uint2 localCoord = DTid.xy;
    float2 uv = float2(localCoord) / float2(cubeFaceSize);
    float3 N = UVToDirection(uv, faceIdx);

    float3 S, T;
	computeBasisVectors(N, S, T);

	// Monte Carlo integration of hemispherical irradiance.
	// As a small optimization this also includes Lambertian BRDF assuming perfectly white surface (albedo of 1.0)
	// so we don't need to normalize in PBR fragment shader (so technically it encodes exitant radiance rather than irradiance).
	float3 irradiance = 0.0;
	for(uint i = 0; i < NumEnvMapSamples; ++i) {
        float2 u  = Razix::Math::ImportanceSampling::HammersleySequence2DFastSample(i, NumEnvMapSamples);
        float3 Li = tangentToWorld(Razix::Math::ImportanceSampling::HemisphereUniformSample(u.x, u.y), N, S, T);
		float cosTheta = max(0.0, dot(Li, N));

		// PIs here cancel out because of division by pdf
		irradiance += 2.0 * EnvCubeMap.SampleLevel(EnvCubeSampler, -Li, 0).rgb * cosTheta;
	}
	irradiance /= float(NumEnvMapSamples);
    IrradianceMap[DTid] = float4(irradiance, 1.0f);
}

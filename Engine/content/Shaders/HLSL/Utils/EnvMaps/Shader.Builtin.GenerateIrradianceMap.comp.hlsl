/*
 * Razix Engine HLSL Compute Shader File
 * Compute shader to convert a cubemap to Irradiance map
 */
//------------------------------------------------------------------------------
#include <Utils/EnvMaps/ShaderInclude.Builtin.EnvmapUtils.hlsl>
//------------------------------------------------------------------------------
// Constants
static const float Epsilon = 0.00001;
static const uint NumSamples = 64 * 1024;
static const float InvNumSamples = 1.0 / float(NumSamples);
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
// Helper functions

// Compute Van der Corput radical inverse
// See: http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float radicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// Sample i-th point from Hammersley point set of NumSamples points total.
float2 sampleHammersley(uint i)
{
	return float2(i * InvNumSamples, radicalInverse_VdC(i));
}

// Uniformly sample point on a hemisphere.
// Cosine-weighted sampling would be a better fit for Lambertian BRDF but since this
// compute shader runs only once as a pre-processing step performance is not *that* important.
// See: "Physically Based Rendering" 2nd ed., section 13.6.1.
float3 sampleHemisphere(float u1, float u2)
{
	const float u1p = sqrt(max(0.0, 1.0 - u1*u1));
	return float3(cos(TwoPI*u2) * u1p, sin(TwoPI*u2) * u1p, u1);
}

// Compute orthonormal basis for converting from tanget/shading space to world space.
void computeBasisVectors(const float3 N, out float3 S, out float3 T)
{
	// Branchless select non-degenerate T.
	T = cross(N, float3(0.0, 1.0, 0.0));
	T = lerp(cross(N, float3(1.0, 0.0, 0.0)), T, step(Epsilon, dot(T, T)));

	T = normalize(T);
	S = normalize(cross(N, T));
}

// Convert point from tangent/shading space to world space.
float3 tangentToWorld(const float3 v, const float3 N, const float3 S, const float3 T)
{
	return S * v.x + T * v.y + N * v.z;
}
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
	for(uint i = 0; i < NumSamples; ++i) {
		float2 u  = sampleHammersley(i);
		float3 Li = tangentToWorld(sampleHemisphere(u.x, u.y), N, S, T);
		float cosTheta = max(0.0, dot(Li, N));

		// PIs here cancel out because of division by pdf
		irradiance += 2.0 * EnvCubeMap.SampleLevel(EnvCubeSampler, -Li, 0).rgb * cosTheta;
	}
	irradiance /= float(NumSamples);
    IrradianceMap[DTid] = float4(irradiance, 1.0f);
}

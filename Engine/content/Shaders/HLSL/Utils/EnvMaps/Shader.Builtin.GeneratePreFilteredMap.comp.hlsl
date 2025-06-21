/*
 * Razix Engine HLSL Compute Shader File
 * Compute shader to convert a CubeMap to Irradiance map
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
#include <Razix/Shared/RZShaderCompitability.h>
#include <Utils/EnvMaps/ShaderInclude.Builtin.EnvmapUtils.h>
//------------------------------------------------------------------------------
TextureCube              EnvCubeMap : register(t0);
SamplerState             EnvCubeSampler : register(s1);
RWTexture2DArray<float4> PreFilteredMap : register(u2);
//------------------------------------------------------------------------------
struct PushConstant
{
    uint2 cubeFaceSize;
    float roughness;
    uint  mipLevel;
};
PUSH_CONSTANT(PushConstant);
//------------------------------------------------------------------------------
// Helper functions
float3 SampleDirUV(float2 st, uint face)
{
    //uv = uv * 2.0f - 1.0f; // Map UV from [0, 1] to [-1, 1]
    float2 uv = (2.0 * float2(st.x, st.y)) - float2(1.0f, 1.0f);
    // Map UV coordinates to 3D direction for each cubemap face
    float3 dir;
    if (face == 0) dir = float3(-1.0, uv.y, uv.x);    // -X
    else if (face == 1)
        dir = float3(1.0, uv.y, -uv.x);    // +X
    else if (face == 2)
        dir = float3(-uv.x, -1.0, -uv.y);    // -Y
    else if (face == 3)
        dir = float3(-uv.x, 1.0, uv.y);    // +Y
    else if (face == 4)
        dir = float3(-uv.x, uv.y, -1.0);    // -Z
    else if (face == 5)
        dir = float3(uv.x, uv.y, 1.0);    // +Z
    return normalize(dir);
}

//------------------------------------------------------------------------------
// TODO: Optimization - use SH
[numthreads(32, 32, 1)] void CS_MAIN(uint3 DTid : SV_DispatchThreadID) {
    // https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/spmap.hlsl

    uint   faceIdx    = DTid.z;    // on CPU we have RHI::Dispatch(W/32, H/32, NUM_FACES = 6)
    uint2  localCoord = DTid.xy;
    float2 uv         = float2(localCoord) / float2(GET_PUSH_CONSTANT(cubeFaceSize));

    float3 N = SampleDirUV(uv, faceIdx);

    float3 S, T;
    computeBasisVectors(N, S, T);

    const float CUBEMAP_LAYERS = 6.0f;
    float       wt             = 4.0f * Razix::Math::PI / (CUBEMAP_LAYERS * (float) GET_PUSH_CONSTANT(cubeFaceSize) * (float) GET_PUSH_CONSTANT(cubeFaceSize));

    float3 color  = 0;
    float  weight = 0;
    float3 Lo     = N;

    // Convolve environment map using GGX NDF importance sampling.
    // Weight by cosine term since Epic claims it generally improves quality.
    for (uint i = 0; i < NumEnvMapSamples; ++i) {
        float2 u  = Razix::Math::ImportanceSampling::HammersleySequence2DFastSample(i, NumEnvMapSamples);
        float3 Lh = tangentToWorld(sampleGGX(u.x, u.y, GET_PUSH_CONSTANT(roughness)), N, S, T);

        // Compute incident direction (Li) by reflecting viewing direction (Lo) around half-vector (Lh).
        float3 Li = 2.0 * dot(Lo, Lh) * Lh - Lo;

        float cosLi = dot(N, Li);
        if (cosLi > 0.0) {
            // Use Mipmap Filtered Importance Sampling to improve convergence.
            // See: https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch20.html, section 20.4

            float cosLh = max(dot(N, Lh), 0.0);

            // GGX normal distribution function (D term) probability density function.
            // Scaling by 1/4 is due to change of density in terms of Lh to Li (and since N=V, rest of the scaling factor cancels out).
            float pdf = ndfGGX(cosLh, GET_PUSH_CONSTANT(roughness)) * 0.25;

            // Solid angle associated with this sample.
            float ws = 1.0 / (NumEnvMapSamples * pdf);

            // Mip level to sample from.
            float mipLevel = max(0.5 * log2(ws / wt) + 1.0, 0.0);

            color += EnvCubeMap.SampleLevel(EnvCubeSampler, -Li, mipLevel).rgb * cosLi;
            weight += cosLi;
        }
    }
    color /= weight;

    PreFilteredMap[DTid] = float4(color, 1.0f);
}

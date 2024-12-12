/*
* Razix Engine GLSL Pixel Shader File
* Pixel shader to convert equirectangular hdr file to cube map
*/
//------------------------------------------------------------------------------
//#define ENABLE_BINDLESS 1
//#include <Common/ShaderInclude.Builtin.BindlessResources.hlsl>
#include <Utils/ShaderInclude.Builtin.Color.h>
//------------------------------------------------------------------------------
// Texture and sampler bindings
//Texture2D HDRTexture : register(t0); // Equirectangular HDR texture
//SamplerState HDRSampler : register(s0); // Sampler for the HDR texture

// Pixel Shader Input
struct PSIn
{
    float3 WorldDir : TEXCOORD0;                    // Cubemap direction
};

// Function to sample the equirectangular map
float2 SampleSphericalMap(float3 v)
{
    const float2 invAtan = float2(0.1591, 0.3183); // Precomputed for atan2 and asin scaling
    float2 uv = float2(atan2(v.z, v.x), asin(v.y)); // Spherical coordinates
    uv *= invAtan; // Scale to [0,1] range
    uv += 0.5;
    return uv;
}

// Pixel Shader Main Function
float4 PS_MAIN(PSIn input) : SV_Target
{
    // Normalize the incoming world direction
    float3 worldDir = normalize(input.WorldDir);

    // Convert the world direction to spherical coordinates for sampling
    //float2 uv = SampleSphericalMap(worldDir);

    // Sample the HDR texture using computed spherical UVs
    //float4 hdrColor = HDRTexture.Sample(HDRSampler, uv);

    // Output the HDR color for this cubemap face
    return float4(RandomColorHash(45), 1.0f);
}
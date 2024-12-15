/*
 * Razix Engine HLSL Compute Shader File
 * Compute shader to convert equi-rectangular hdr file to cube map, write directly to the cubemap
 */
//------------------------------------------------------------------------------
// Constants
// Global View Projection matrices
#define PI 3.14159265369f

float3 UVToDirection(float2 uv, uint face)
{
    uv = uv * 2.0f - 1.0f; // Map UV from [0, 1] to [-1, 1]

    // Map UV coordinates to 3D direction for each cubemap face
    if (face == 0) return float3(1.0, -uv.y, -uv.x);  // +X
    if (face == 1) return float3(-1.0, -uv.y, uv.x);  // -X
    if (face == 2) return float3(uv.x, 1.0, uv.y);    // +Y
    if (face == 3) return float3(uv.x, -1.0, -uv.y);  // -Y
    if (face == 4) return float3(uv.x, -uv.y, 1.0);   // +Z
    return float3(-uv.x, -uv.y, -1.0);                // -Z
}

float2 DirectionToEquirectangularUV(float3 dir)
{
    // Normalize direction
    dir = normalize(dir);

    // Convert 3D direction to UV coordinates
    float phi = atan2(dir.z, dir.x);
    float theta = asin(dir.y);

    float2 uv;
    uv.x = 0.5f + phi / (2.0f * PI); // Map to [0, 1]
    uv.y = 0.5f - theta / PI;       // Map to [0, 1]
    return uv;
}

//------------------------------------------------------------------------------
Texture2D HDRTexture : register(t0); // Equirectangular HDR texture
SamplerState HDRSampler : register(s1); // Sampler for the HDR texture
RWTexture2DArray<float4> CubeMapRT: register(u2);
//------------------------------------------------------------------------------
cbuffer Constants : register(b3)
{
    uint2 cubeFaceSize;
    uint mipLevel;
};
//------------------------------------------------------------------------------
[numthreads(32, 32, 1)]
void CS_MAIN(uint3 DTid: SV_DispatchThreadID)
{
    uint faceIdx = DTid.z; // on CPU we have RHI::Dispatch(W, H, NUM_FACES = 6)
    uint2 localCoord = DTid.xy;
    float2 uv = (localCoord + 0.5f) / cubeFaceSize;

    float3 direction = UVToDirection(uv, faceIdx);
    float2 eqUV = DirectionToEquirectangularUV(direction);
    float4 hdrColor = HDRTexture.SampleLevel(HDRSampler, eqUV, mipLevel);
    
    CubeMapRT[uint3(localCoord, faceIdx)] = hdrColor;
}

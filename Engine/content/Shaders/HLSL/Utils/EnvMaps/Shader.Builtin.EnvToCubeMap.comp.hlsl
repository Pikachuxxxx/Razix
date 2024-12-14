/*
 * Razix Engine HLSL Compute Shader File
 * Compute shader to convert equi-rectangular hdr file to cube map, write directly to the cubemap
 */
//------------------------------------------------------------------------------
Texture2D HDRTexture : register(t1); // Equirectangular HDR texture
SamplerState HDRSampler : register(s2); // Sampler for the HDR texture
RWTexture2DArray<float4> CubeMapRT: register(u3);
//------------------------------------------------------------------------------
cbuffer Constants : register(b0)
{
    uint cubeFaceSize; // Width and height of each cubemap face
    uint mipLevel;     // Current mipmap level
};
//------------------------------------------------------------------------------
[numthreads(32, 32, 1)]
void CS_MAIN(uint3 DTid: SV_DispatchThreadID)
{
    uint faceIdx = DTid.z; // on CPU we have RHI::Dispatch(W, H, NUM_FACES = 6)
    uint2 localCoord = DTid.xy;
    float2 uv = (localCoord + 0.5f) / cubeFaceSize;
    
    CubeMapRT[uint3(localCoord, faceIdx)] = float4(uv, 0.0f, 1.0f);
}

/*
 * Razix Engine GLSL Vertex Shader File
 * Pixel Shader to render the G-Buffer render targets
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//-------------------------------
#include <Material/ShaderInclude.Builtin.Material.h>
//-------------------------------
// Color Utils (Debug only)
#include <Utils/ShaderInclude.Builtin.Color.h>
//------------------------------------------------------------------------------ 
struct PSIn
{
    float4 WorldPos   : POSITION0;
    float4 Color      : COLOR0;
    float2 UV         : TEXCOORD0;
    float3 Normal     : NORMAL0;
    float3 Tangent    : TANGENT0;
};

struct PSOut
{
     float4 GBuffer0 : SV_Target0;
     float4 GBuffer1 : SV_Target1;
     float4 GBuffer2 : SV_Target2;
};
//------------------------------------------------------------------------------
PSOut PS_MAIN(PSIn input, uint primitiveID: SV_PrimitiveID)
{
    float2 uv = float2(input.UV.x, input.UV.y);

    float3 albedo = Mat_getAlbedoColor(uv);
    float metallic = Mat_getMetallicColor(uv);
    float roughness = Mat_getRoughnessColor(uv);
    float ao = Mat_getAOColor(uv);

    // Write the Normals to the GBuffer0 
#ifdef __GLSL__
    // Due to viewport flipping in vulkan, flip normals too
    //input.Normal.y = -input.Normal.y;
#endif
    float3 N_Surface = normalize(input.Normal);
    float3 N = normalize(Mat_getNormalMapNormals(uv, input.WorldPos.xyz, N_Surface));

    PSOut output;

    output.GBuffer0 = float4(N, metallic);
    output.GBuffer1 = float4(albedo.rgb, roughness);
    output.GBuffer2 = float4(input.WorldPos.xyz, ao);

    return output;
}
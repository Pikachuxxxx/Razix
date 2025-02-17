/*
 * Razix Engine GLSL Vertex Shader File
 * Pixel Shader to render the G-Buffer render targets
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
#include <Material/ShaderInclude.Builtin.Material.h>
//------------------------------------------------------------------------------ 
struct PSIn
{
    float4 Position   : SV_Position;
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
PSOut PS_MAIN(PSIn input)
{
    float3 albedo = Mat_getAlbedoColor(input.UV);
    float metallic = Mat_getMetallicColor(input.UV);
    float roughness = Mat_getRoughnessColor(input.UV);
    float ao = Mat_getAOColor(input.UV);

    // Write the Normals to the GBuffer0  
    float3 N_Surface = normalize(input.Normal);
    float3 N = normalize(Mat_getNormalMapNormals(input.UV, input.Position.xyz, N_Surface));

    PSOut output;
    output.GBuffer0 = float4(N, metallic);
    output.GBuffer1 = float4(albedo.rgb, roughness);
    output.GBuffer2 = float4(input.Position.xyz, ao);

    return output;
}
//------------------------------------------------------------------------------
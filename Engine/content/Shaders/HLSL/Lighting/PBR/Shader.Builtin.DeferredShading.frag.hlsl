/*
 * Razix Engine HLSL Pixel Shader File
 * Calculate the Final PBR direct lighting 
 */
//------------------------------------------------------------------------------
#include <ShaderInclude.Builtin.ShaderLangCommon.h>
//------------------------------------------------------------------------------
// GBuffer Deferred resource bindings
#include <Rendering/ShaderInclude.Builtin.DeferredPassBindings.h>
#include <Rendering/FX/ShaderInclude.Builtin.TonemappingFuncs.h>
//-------------------------------
// PBR - BRDF helper functions
#include <Lighting/PBR/ShaderInclude.Builtin.CookTorranceBRDF.h>
//------------------------------------------------------------------------------
// Final Render targets 
struct PSOut
{
    float4 SceneColor: SV_Target0;
};
//------------------------------------------------------------------------------
PSOut PS_MAIN(VSOutput input)
{
    // Flip UVs since the quad is also flipped and so are it's UVs
    float2 uv = float2(input.UV.x, 1.0f - input.UV.y);

    float4 Normal_Metallic = gBuffer0.Sample(linearSampler, uv);
    float4 Albedo_Roughness = gBuffer1.Sample(linearSampler, uv);
    float4 Pos_AO = gBuffer2.Sample(linearSampler, uv);

    float3 albedo   = Albedo_Roughness.rgb;
    float roughness = Albedo_Roughness.a;
    float metallic  = Normal_Metallic.a;
    float3 worldPos = Pos_AO.rgb;
    float ao        = Pos_AO.a;

    float3 N = Normal_Metallic.rgb;
    float3 V = normalize((GET_PUSH_CONSTANT(camViewPos) - worldPos));
    float3 R = reflect(-V, N);

    // calculate reflectance at normal incidence; if dielectric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    float3 F0 = float3(0.04f, 0.04f, 0.04f); 
    F0 = lerp(F0, albedo, metallic);

    // Rendering equation - solve for BRDF
    // Direct Lighting - output radiance
    float3 Lo = float3(0.0f, 0.0f, 0.0f);

    for(int i = 0; i < numLights; ++i)
    {
        LightData light = lightData[i];

        float3 L;
        float attenuation = 0.0f;

        if(light.type == DIRECTIONAL) {
            L = normalize(light.position.xyz);
            attenuation = 1.0f;
        }
        else if(light.type == POINT) {
            L = normalize(light.position - worldPos);
            float distance    = length(light.position - worldPos);
            attenuation = 1.0f / (distance * distance);
        }
        float3 H = normalize(L + V);
        float3 Li = light.color * attenuation;

        // calculate BRDF
        float3 brdf = CookTorranceBRDF(L, V, N, F0, albedo, metallic, roughness);

        float NdotL = max(dot(N, L), 0.0);                
        Lo += brdf * Li * NdotL;
    }

    // IBL ambient lighting (diffuse + specular contribution)
    float3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0f), F0, roughness);
    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    
    float3 irradiance = IrradianceMap.Sample(cubeSampler, N).rgb;
    float3 diffuseIndirect = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 5.0f;
    float3 prefilteredColor = PreFilteredMap.SampleLevel(cubeSampler, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    float2 brdf  = BrdfLUT.Sample(linearSampler, float2(max(dot(N, V), 0.0), roughness)).rg;
    float3 specularIndirect = prefilteredColor * (F * brdf.x + brdf.y);

    float3 ambientLighting = (kD * diffuseIndirect + specularIndirect) * ao;
    float3 color = ambientLighting + Lo;

    // Tonemap here cause I'm lazy to add another pass
    color = TonemapFuncs::Filmic(color);
    // Gamma correction
    float gammaFactor = 1.0f / 2.2f;
    color = pow(color, float3(gammaFactor, gammaFactor, gammaFactor)); 
	
    PSOut output;
    output.SceneColor = float4(color, 1.0f);

    return output;
}
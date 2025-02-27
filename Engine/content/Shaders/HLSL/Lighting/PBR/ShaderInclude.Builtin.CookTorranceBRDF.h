#ifndef _COOK_TORRANCE_BRDF_GLSL_
#define _COOK_TORRANCE_BRDF_GLSL_
//----------------------------------------------------------------------------
#include <Razix/Shared/RZShaderCompitability.h>
//----------------------------------------------------------------------------
// https://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
// Note:- a2 = alphaRoughness = (roughness * roughness)
//----------------------------------------------------------------------------
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = fmax(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom       = Razix::Math::PI * denom * denom;

    return nom / denom;
}
//----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = roughness;    //(r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
//----------------------------------------------------------------------------
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
//----------------------------------------------------------------------------
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
//----------------------------------------------------------------------------
float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0f - roughness, 1.0f - roughness, 1.0f - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 7.5);
}
//----------------------------------------------------------------------------
float3 CookTorranceBRDF(float3 L, float3 V, float3 N, float3 F0, float3 albedo, float metallic, float roughness)
{
    // Outgoing reflectance for the given light
    float3 Lo = float3(0.0f, 0.0f, 0.0f);

    float3 H = normalize(V + L);

    // Cook-Torrance BRDF
    float  NDF = DistributionGGX(N, H, roughness);
    float  G   = GeometrySmith(N, V, L, roughness);
    float3 F   = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    float3 numerator   = NDF * G * F;
    float  denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;    // + 0.0001 to prevent divide by zero
    float3 specular    = numerator / denominator;

    // kS is equal to Fresnel
    float3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    // multiply kD by the inverse metalness such that only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // return BRDF
    return (kD * albedo / Razix::Math::PI + specular);
}
#endif
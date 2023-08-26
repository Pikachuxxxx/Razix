#ifndef _PBR_DIRECT_LIGHTING_GLSL_
#define _PBR_DIRECT_LIGHTING_GLSL_

#include <Lighting/PBR/ShaderInclude.Builtin.BRDF.glsl>

/**
 * Calculate the Outgoing radince for a give light and view position
 * 
 * @param L The Light Direction
 * @param V The View positon of the camera/viewer
 * @param N Normal vector of the surface
 * @param F0 Reflectance and normal incidence
 */
vec3 CalculateRadiance(vec3 L, vec3 V, vec3 N, vec3 F0, vec3 albedo, float metallic, float roughness, vec3 lightColor, float attenuation)
{
    // Outgoing reflectance for the given light
    vec3 Lo = vec3(0.0);

    vec3 H = normalize(V + L);
    vec3 radiance = lightColor * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;
        
    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;	  

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);        

    // return outgoing radiance Lo
    Lo = (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    return Lo;
}

#endif

#ifndef _PBR_DIRECT_LIGHTING_GLSL_
#define _PBR_DIRECT_LIGHTING_GLSL_

#include <Lighting/PBR/BRDF.glsl>

LightContribution PBR_DirectLighting(vec3 radiance, vec3 diffuseColor, vec3 F0,
                                     float specularWeight, float a, float NdotV,
                                     float NdotL, float NdotH, float VdotH) {
  const vec3 F = F_Schlick(F0, vec3(1.0), VdotH);

  // Lambertian BRDF
  const vec3 diffuse = (1.0 - specularWeight * F) * (diffuseColor / PI);

  // Cook-Torrance BRDF
  const float Vis = V_GGX(NdotV, NdotL, a);
  const float D = D_GGX(NdotH, a);
  const vec3 specular = specularWeight * F * Vis * D;

  return LightContribution(diffuse * radiance, specular * radiance);
}

#endif

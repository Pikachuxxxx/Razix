#ifndef _IBL_AMBIENT_LIGHTING_GLSL_
#define _IBL_AMBIENT_LIGHTING_GLSL_

#include <Lighting/PBR/BRDF.glsl>
#include <Utils/Texture.glsl>

LightContribution IBL_AmbientLighting(vec3 diffuseColor, vec3 F0,
                                      float specularWeight, float roughness,
                                      vec3 N, vec3 V, float NdotV) {
  const vec2 f_ab = texture(BRDF, clamp01(vec2(NdotV, roughness))).rg;
  const vec3 Fr = max(vec3(1.0 - roughness), F0) - F0;
  const vec3 k_S = F0 + Fr * pow(1.0 - NdotV, 5.0);

  // -- Diffuse IBL:

  const vec3 irradiance = textureLod(IrradianceMap, N, 0.0).rgb;

  vec3 FssEss = specularWeight * k_S * f_ab.x + f_ab.y;

  // Multiple scattering, from Fdez-Aguera
  const float Ems = (1.0 - (f_ab.x + f_ab.y));
  const vec3 F_avg = specularWeight * (F0 + (1.0 - F0) / 21.0);
  const vec3 FmsEms = Ems * FssEss * F_avg / (1.0 - F_avg * Ems);
  const vec3 k_D = diffuseColor * (1.0 - FssEss + FmsEms);

  const vec3 diffuse = (FmsEms + k_D) * irradiance;

  // -- Specular IBL:

  const vec3 R = reflect(-V, N);

  const float maxReflectionLOD = calculateMipLevels(PrefilteredEnvMap);
  const vec3 prefilteredColor =
    textureLod(PrefilteredEnvMap, R, roughness * maxReflectionLOD).rgb;

  FssEss = k_S * f_ab.x + f_ab.y;

  const vec3 specular = specularWeight * prefilteredColor * FssEss;

  return LightContribution(diffuse, specular);
}

#endif

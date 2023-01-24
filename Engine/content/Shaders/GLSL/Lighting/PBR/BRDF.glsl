#ifndef _BRDF_GLSL_
#define _BRDF_GLSL_

#include <Common/Math.glsl>

// NOTE: a = alphaRoughness = (roughness * roughness)

vec3 F_Schlick(vec3 F0, vec3 F90, float VdotH) {
  return F0 + (F90 - F0) * pow(max(1.0 - VdotH, 0.0), 5.0);
}
vec3 F_Schlick(vec3 F0, vec3 F90, float VdotH, float roughness) {
  return F0 + (max(vec3(F90 - roughness), F0) - F0) *
                pow(max(1.0 - VdotH, 0.0), 5.0);
}

float V_GGX(float NdotV, float NdotL, float a) {
  const float a2 = a * a;
  const float GGXV = NdotL * sqrt(NdotV * NdotV * (1.0 - a2) + a2);
  const float GGXL = NdotV * sqrt(NdotL * NdotL * (1.0 - a2) + a2);
  const float GGX = GGXV + GGXL;

  return GGX > 0.0 ? 0.5 / GGX : 0.0;
}

float V_SmithGGXCorrelated(float NdotV, float NdotL, float roughness) {
  const float a2 = pow(roughness, 4.0);
  const float GGXV = NdotL * sqrt(NdotV * NdotV * (1.0 - a2) + a2);
  const float GGXL = NdotV * sqrt(NdotL * NdotL * (1.0 - a2) + a2);
  return 0.5 / (GGXV + GGXL);
}

float D_GGX(float NdotH, float a) {
  const float a2 = a * a;
  const float f = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
  return a2 / (PI * f * f);
}

#endif

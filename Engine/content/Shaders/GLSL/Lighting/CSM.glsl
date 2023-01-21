#ifndef _CSM_GLSL_
#define _CSM_GLSL_

#include <Resources/Cascades.glsl>

float _getDirLightVisibility(uint cascadeIndex, vec3 fragPos, float NdotL) {
  vec4 shadowCoord =
    u_Cascades.viewProjMatrices[cascadeIndex] * vec4(fragPos, 1.0);

  const float bias = 0.0;

#if !SOFT_SHADOWS
  return texture(t_CascadedShadowMaps,
                 vec4(shadowCoord.xy, cascadeIndex, shadowCoord.z - bias));
#else
  const ivec2 shadowMapSize = textureSize(t_CascadedShadowMaps, 0).xy;
  const float kScale = 1.0;
  const float dx = kScale * 1.0 / float(shadowMapSize.x);
  const float dy = kScale * 1.0 / float(shadowMapSize.y);

  const int kRange = 1;
  float shadowFactor = 0.0;
  uint count = 0;
  for (int x = -kRange; x <= kRange; ++x) {
    for (int y = -kRange; y <= kRange; ++y) {
      shadowFactor += texture(t_CascadedShadowMaps,
                              vec4(shadowCoord.xy + vec2(dx * x, dy * y),
                                   cascadeIndex, shadowCoord.z - bias));
      count++;
    }
  }
  return shadowFactor / float(count);
#endif
}

#endif

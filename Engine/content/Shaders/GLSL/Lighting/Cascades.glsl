#ifndef _CASCADES_GLSL_
#define _CASCADES_GLSL_

#define MAX_NUM_CASCADES 4

layout(set = 2, binding = 0, std140) uniform Cascades {
  vec4 splitDepth;
  mat4 viewProjMatrices[MAX_NUM_CASCADES];
}u_Cascades;

uint _selectCascadeIndex(vec3 fragPosViewSpace) {
  uint cascadeIndex = 0;
  for (uint i = 0; i < MAX_NUM_CASCADES - 1; ++i)
    if (fragPosViewSpace.z < u_Cascades.splitDepth[i]) cascadeIndex = i + 1;
  return cascadeIndex;
}
#endif

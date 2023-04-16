#ifndef _LPV_GLSL_
#define _LPV_GLSL_

#include <Common/ShaderInclude.Math.glsl>

#define SH_C0 0.282094791 // 1 / 2sqrt(pi)
#define SH_C1 0.488602512 // sqrt(3/pi) / 2

vec4 SH_evaluate(vec3 direction) {
  direction = normalize(direction);
  return vec4(SH_C0, -SH_C1 * direction.y, SH_C1 * direction.z,
              -SH_C1 * direction.x);
}

#define SH_cosLobe_C0 0.886226925 // sqrt(pi)/2
#define SH_cosLobe_C1 1.02332671  // sqrt(pi/3)

vec4 SH_evaluateCosineLobe(vec3 direction) {
  direction = normalize(direction);
  return vec4(SH_cosLobe_C0, -SH_cosLobe_C1 * direction.y,
              SH_cosLobe_C1 * direction.z, -SH_cosLobe_C1 * direction.x);
}

struct SHcoeffs {
  vec4 red, green, blue;
};

#endif

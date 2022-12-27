#ifndef _MATH_CONSTANTS_GLSL
#define _MATH_CONSTANTS_GLSL

// -- CONSTANTS:

#define PI 3.1415926535897932384626433832795
#define TWO_PI 6.2831853071795864769252867665590
#define HALF_PI 1.5707963267948966192313216916398
#define EPSILON 0.00001

// -- FUNCTIONS:

#define clamp01(x) clamp(x, 0.0, 1.0)

float max3(vec3 v) { return max(max(v.x, v.y), v.z); }

float lerp(float a, float b, float t) { return (1.0 - t) * a + b * t; }
float invlerp(float a, float b, float v) { return (v - a) / (b - a); }

float remap(vec2 i, vec2 o, float v) {
  const float t = invlerp(i.x, i.y, v);
  return lerp(o.x, o.y, t);
}

bool isApproximatelyEqual(float a, float b) {
  return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

float random(vec2 co) {
  // http://byteblacksmith.com/improvements-to-the-canonical-one-liner-glsl-rand-for-opengl-es-2-0/
  const float a = 12.9898;
  const float b = 78.233;
  const float c = 43758.5453;
  const float dt = dot(co, vec2(a, b));
  const float sn = mod(dt, PI);
  return fract(sin(sn) * c);
}

// https://softwareengineering.stackexchange.com/questions/212808/treating-a-1d-data-structure-as-2d-grid/212813

uint flatten2D(uvec2 id, uint width) { return id.x + width * id.y; }
uvec2 unflatten2D(uint i, uint width) { return uvec2(i % width, i / width); }

uint flatten3D(uvec3 id, uvec2 size) {
  return id.x + size.x * id.y + size.x * size.y * id.z;
}
uvec3 unflatten3D(uint i, uvec2 size) {
  return uvec3(i % size.x, (i / size.x) % size.y, i / (size.x * size.y));
}

#endif

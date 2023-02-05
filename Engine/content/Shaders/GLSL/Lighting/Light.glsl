#ifndef _LIGHT_GLSL_
#define _LIGHT_GLSL_

//------------------------------------------------------
// Constants and Defines
// Max no of lights in the scene
#define MAX_LIGHTS 1024

// Type of the Light (enum)
const uint LightType_Directional = 0;
const uint LightType_Point = 1;
const uint LightType_Spot = 2;

//------------------------------------------------------
// Light Data
// The light info that every light stores in the scene
struct LightData {
    vec3 position;  // [point/spot] .xyz = position, .w = range
    float range;
    vec3 color;     // .rgb = color, .a = intensity
    float intensity;
    vec4 direction; // [spot/directional] from light, normalized
    float constant;
    float linear;
    float quadratic;
    float innerConeAngle; // [spot] in radians
    float outerConeAngle; // [spot] in radians
    uint type;
};
//------------------------------------------------------
// GPU Light
// The GPU lights data that will be uploaded to the 
#define DECLARE_LIGHT_BUFFER(st, index, name)                                   \
layout(set = st, binding = index) uniform LightBuffer {                         \
    uint numLights;                                                           \
    LightData data[];                                                           \
}                                                                                \
name;
//------------------------------------------------------
// Utility DS and Functions
struct LightContribution {
    vec3 diffuse;
    vec3 specular;
};
//------------------------------------------------------
float _getLightRange(const in LightData light) { return light.range; }
//------------------------------------------------------
float _getLightAttenuation(const in LightData light, vec3 fragToLight) {
  if (light.type == LightType_Directional) return 1.0;

  const float d = length(fragToLight);
  const float lightRange = _getLightRange(light);
  if (d > lightRange) return 0.0;

#if 1
  // https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_lights_punctual/README.md
  const float rangeAttenuation =
    max(min(1.0 - pow(d / lightRange, 4.0), 1.0), 0.0) / pow(d, 2.0);
#else
  // Real Shading in Unreal Engine 4 (page 12)
  const float rangeAttenuation =
    pow(clamp01(1.0 - pow(d / lightRange, 4.0)), 2.0) / (d * d + 1.0);
#endif

  float spotAttenuation = 1.0;
  if (light.type == LightType_Spot) {
    const float innerConeCos = cos(light.innerConeAngle);
    const float outerConeCos = cos(light.outerConeAngle);
    const float actualCos =
      dot(normalize(light.direction.xyz), normalize(-fragToLight));
    if (actualCos > outerConeCos) {
      if (actualCos < innerConeCos)
        spotAttenuation = smoothstep(outerConeCos, innerConeCos, actualCos);
    } else
      spotAttenuation = 0.0;
  }

  return rangeAttenuation * spotAttenuation;
}
//------------------------------------------------------
vec3 _getLightIntensity(const in LightData light, vec3 fragToLight) {
  return light.color.rgb * light.intensity *
         _getLightAttenuation(light, fragToLight);
}
//------------------------------------------------------
#endif
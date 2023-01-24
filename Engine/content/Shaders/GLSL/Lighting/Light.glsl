#ifndef _LIGHT_GLSL_
#define _LIGHT_GLSL_

const uint LightType_Directional = 0;
const uint LightType_Spot = 1;
const uint LightType_Point = 2;

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    float time;
};
struct PointLight
{
    vec3  position;
    vec3  color;
    float radius;
    float constant;
    float linear;
    float quadratic;
};
struct SpotLight
{
    vec3  position;
    vec3  direction;
    vec3  color;
    float radius;
    float constant;
    float linear;
    float quadratic;
};
struct LightData
{
    int                 type;
    DirectionalLight    dirLightData;
    PointLight          pointLightData;
    SpotLight           spotLightData;
};

// GPULight in UploadLights.cpp
struct Light {
  vec4 position;  // [point/spot] .xyz = position, .w = range
  vec4 direction; // [spot/directional] from light, normalized
  vec4 color;     // .rgb = color, .a = intensity
  uint type;
  float innerConeAngle; // [spot] in radians
  float outerConeAngle; // [spot] in radians
  // Implicit padding, 4bytes
};

#define _DECLARE_LIGHT_BUFFER(st, index, name)                                     \
  layout(set = st, binding = index, std430) restrict readonly buffer LightBuffer {       \
    uint numLights;                                                            \
    uint _pad[3];                                                              \
    Light data[];                                                              \
  }                                                                            \
  name

struct LightContribution {
  vec3 diffuse;
  vec3 specular;
};

// -- FUNCTIONS:

float _getLightRange(const in Light light) { return light.position.w; }

float _getLightAttenuation(const in Light light, vec3 fragToLight) {
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

vec3 _getLightIntensity(const in Light light, vec3 fragToLight) {
  return light.color.rgb * light.color.a *
         _getLightAttenuation(light, fragToLight);
}

#endif
#version 450 core

 #extension GL_ARB_shading_language_420pack : enable
////////////////////////////////////////////////////////////////////////////////
// Constants, Input, Output and Uniforms

const float PI = 3.14159265359;
// -----------------------------------------------------------------------------
// Input from the Vertex Shader
in VS_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
} vs_in;

// -----------------------------------------------------------------------------
// Uniforms
// material parameters
layout(binding = 0) uniform sampler2D albedoMap;
layout(binding = 1) uniform sampler2D normalMap;
layout(binding = 2) uniform sampler2D metallicMap;
layout(binding = 3) uniform sampler2D RoughnessMap;
layout(binding = 4) uniform sampler2D aoMap;
layout(binding = 5) uniform samplerCube prefilterMap;
layout(binding = 6) uniform sampler2D brdfLUT;
layout(binding = 7) uniform samplerCube irradianceMap;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

// Camera
uniform vec3 camPos;
// -----------------------------------------------------------------------------
// Final Output color of the Fragment
out vec4 FragColor;

////////////////////////////////////////////////////////////////////////////////
// Misc/Processing functions
// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, vs_in.TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(vs_in.FragPos);
    vec3 Q2  = dFdy(vs_in.FragPos);
    vec2 st1 = dFdx(vs_in.TexCoords);
    vec2 st2 = dFdy(vs_in.TexCoords);

    vec3 N   = normalize(vs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
////////////////////////////////////////////////////////////////////////////////
// BRDF Functions
// -----------------------------------------------------------------------------
// Normal Distribution Functions
//      TrowBridge-Reitz GGX

float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;

    return nom / denom;
}
// -----------------------------------------------------------------------------
// Geometry Distribution Functions
//      Shlick GGX
float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// -----------------------------------------------------------------------------
//      Smith GGX
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1  = GeometrySchlickGGX(NdotV, k);
    float ggx2  = GeometrySchlickGGX(NdotL, k);

    return ggx1 * ggx2;
}
// -----------------------------------------------------------------------------
// Frensel Functions
//      Frensel Shlick
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    // cosTheta being the dot product result between the surface’s normal n and the halfway h (or view v) direction
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}


////////////////////////////////////////////////////////////////////////////////
// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
vec3 aces(vec3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// -----------------------------------------------------------------------------
// Filmic Tonemapping Operators http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 tonemapFilmic(vec3 x) {
  vec3 X = max(vec3(0.0), x - 0.004);
  vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
  return pow(result, vec3(2.2));
}

////////////////////////////////////////////////////////////////////////////////
// Tonemapping
// Lottes 2016, "Advanced Techniques and Optimization of HDR Color Pipelines"
vec3 lottes(vec3 x) {
  const vec3 a = vec3(1.6);
  const vec3 d = vec3(0.977);
  const vec3 hdrMax = vec3(8.0);
  const vec3 midIn = vec3(0.18);
  const vec3 midOut = vec3(0.267);

  const vec3 b =
      (-pow(midIn, a) + pow(hdrMax, a) * midOut) /
      ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);
  const vec3 c =
      (pow(hdrMax, a * d) * pow(midIn, a) - pow(hdrMax, a) * pow(midIn, a * d) * midOut) /
      ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

  return pow(x, a) / (pow(x, a * d) * b + c);
}
////////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// Unreal
// Unreal 3, Documentation: "Color Grading"
// Adapted to be close to Tonemap_ACES, with similar range
// Gamma 2.2 correction is baked in, don't use with sRGB conversion!
vec3 unreal(vec3 x) {
  return x / (x + 0.155) * 1.019;
}

float unreal(float x) {
  return x / (x + 0.155) * 1.019;
}

// Main Function
void main()
{
    vec3 albedo     = texture(albedoMap, vs_in.TexCoords).rgb;//, vec3(2.2));pow(
    float metallic  = texture(metallicMap, vs_in.TexCoords).r;
    float roughness = texture(RoughnessMap, vs_in.TexCoords).r;
    float ao        = texture(aoMap, vs_in.TexCoords).r;

    vec3 N = getNormalFromMap();
    // vec3 N = vs_in.Normal;
    vec3 V = normalize(camPos - vs_in.FragPos);
    vec3 R = reflect(-V, N);

    // calculate reflectance at normal incidence; if dia-electric (like plastic)
    // use F0 of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
     vec3 Lo = vec3(0.0);
     for(int i = 0; i < 4; ++i)
     {
         // calculate per-light radiance
         vec3 L              = normalize(lightPositions[i] - vs_in.FragPos);
         vec3 H              = normalize(V + L);
         float distance      = length(lightPositions[i] - vs_in.FragPos);
         float attenuation   = 1.0 / (distance * distance);
         vec3 radiance       = lightColors[i] * attenuation;
    
         // Cook-Torrance BRDF
         float NDF = DistributionGGX(N, H, roughness);
         float G   = GeometrySmith(N, V, L, roughness);
         vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
    
         vec3 numerator      = NDF * G * F;
         float denominator   = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
         vec3 specular       = numerator / denominator;
    
         // kS is equal to Fresnel
         vec3 kS = F;
    
         // for energy conservation, the diffuse and specular light can't
         // be above 1.0 (unless the surface emits light); to preserve this
         // relationship the diffuse component (kD) should equal 1.0 - kS
         vec3 kD = vec3(1.0) - kS;
    
         // multiply kD by the inverse metalness such that only non-metals
         // have diffuse lighting, or a linear blend if partly metal (pure metals
         // have no diffuse light)
         kD *= 1.0 - metallic;
    
         // scale light by NdotL
         float NdotL = max(dot(N, L), 0.0);
    
         // Add to outgoing radiance Lo
         Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
     }

    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0f - metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse    = irradiance * albedo;

    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor   = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf               = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular           = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = Lo + ((kD * diffuse) + specular) * ao;

    // Tonemapping
    vec3 color = lottes(ambient);

    // Gamma correction
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);

    // FragColor = vec4(vs_in.TexCoords, 0.0f, 1.0);
    //FragColor = vec4(texture(albedoMap, vs_in.TexCoords));//vec4(ao, roughness, metallic, 1.0f);
}
////////////////////////////////////////////////////////////////////////////////

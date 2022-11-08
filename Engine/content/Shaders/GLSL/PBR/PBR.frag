#version 410 core

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
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;
// -----------------------------------------------------------------------------
// Final Output color of the Fragment
out vec4 FragColor;

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

// Main Function
void main()
{

    vec3 N = normalize(vs_in.Normal);
    vec3 V = normalize(camPos - vs_in.FragPos);

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

    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    // Tonemapping
    color = lottes(color);

    // Gamma correction
    color = pow(color, vec3(1.0/2.2));

    // FragColor = vec4(vs_in.Normal, 1.0f); // Testing Normal cooridinates
    FragColor = vec4(color, 1.0);
}
////////////////////////////////////////////////////////////////////////////////

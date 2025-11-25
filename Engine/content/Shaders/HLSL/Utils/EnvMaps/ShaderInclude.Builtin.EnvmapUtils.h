#ifndef CUBE_MAP_UTILS
#define CUBE_MAP_UTILS

#include <Razix/Math/MathConstants.h>
//------------------------------------------------------------------------------
// Reference: https://github.com/Nadrin/PBR/

#define CUBEMAP_DIM         1024
#define IRRADIANCE_MAP_DIM  64
#define PREFILTERED_MAP_DIM 128

static const uint  NumIrradianceMapSamples    = 64 * CUBEMAP_DIM;
static const float InvNumIrradianceMapSamples = 1.0 / float(NumIrradianceMapSamples);
// Use for both envmap and specular rpefiltered map sampling
static const uint  NumEnvMapSamples    = 1024;
static const float InvNumEnvMapSamples = 1.0 / float(NumEnvMapSamples);

// Global View Projection matrices
float3 UVToDirection(float2 st, uint face)
{
    //uv = uv * 2.0f - 1.0f; // Map UV from [0, 1] to [-1, 1]
    float2 uv = (2.0 * float2(st.x, 1.0 - st.y)) - float2(1.0f, 1.0f);
    // Map UV coordinates to 3D direction for each cubemap face
    float3 dir;
    if (face == 0) dir = float3(-1.0, uv.y, uv.x);    // -X
    else if (face == 1)
        dir = float3(1.0, uv.y, -uv.x);    // +X
    else if (face == 2)
        dir = float3(-uv.x, 1.0, uv.y);    // +Y
    else if (face == 3)
        dir = float3(-uv.x, -1.0, -uv.y);    // -Y
    else if (face == 4)
        dir = float3(-uv.x, uv.y, -1.0);    // -Z
    else if (face == 5)
        dir = float3(uv.x, uv.y, 1.0);    // +Z
    return normalize(dir);
}

float2 DirectionToEquirectangularUV(float3 dir)
{
    // Normalize direction
    dir = normalize(dir);

    // Convert 3D direction to UV coordinates
    float phi   = atan2(dir.z, dir.x);
    float theta = asin(dir.y);

    float2 uv;
    uv.x = 0.5f + phi / (2.0f * Razix::Math::PI);    // Map to [0, 1]
    uv.y = 0.5f - theta / Razix::Math::PI;           // Map to [0, 1]
    return uv;
}

// Compute orthonormal basis for converting from tanget/shading space to world space.
void computeBasisVectors(const float3 N, out float3 S, out float3 T)
{
    // Branchless select non-degenerate T.
    T = cross(N, float3(0.0, 1.0, 0.0));
    T = lerp(cross(N, float3(1.0, 0.0, 0.0)), T, step(Razix::Math::Epsilon, dot(T, T)));

    T = normalize(T);
    S = normalize(cross(N, T));
}

// Convert point from tangent/shading space to world space.
float3 tangentToWorld(const float3 v, const float3 N, const float3 S, const float3 T)
{
    return S * v.x + T * v.y + N * v.z;
}

// Importance sample GGX normal distribution function for a fixed roughness value.
// This returns normalized half-vector between Li & Lo.
// For derivation see: http://blog.tobias-franke.eu/2014/03/30/notes_on_importance_sampling.html
float3 sampleGGX(float u1, float u2, float roughness)
{
    float alpha = roughness * roughness;

    float cosTheta = sqrt((1.0 - u2) / (1.0 + (alpha * alpha - 1.0) * u2));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);    // Trig. identity
    float phi      = Razix::Math::TwoPI * u1;

    // Convert to Cartesian upon return.
    return float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness)
{
    float alpha   = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (Razix::Math::PI * denom * denom);
}

#endif

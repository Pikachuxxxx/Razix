#ifndef CUBE_MAP_UTILS
#define CUBE_MAP_UTILS
//------------------------------------------------------------------------------
// Reference: https://github.com/Nadrin/PBR/
// Constants
static const float PI = 3.1415926539f;
static const float TwoPI = 2 * PI;
// Global View Projection matrices
float3 UVToDirection(float2 uv, uint face)
{
    uv = uv * 2.0f - 1.0f; // Map UV from [0, 1] to [-1, 1]

    // Map UV coordinates to 3D direction for each cubemap face
    float3 dir;
    if (face == 0) dir = float3(1.0, -uv.y, -uv.x);  // +X
    if (face == 1) dir = float3(-1.0, -uv.y, uv.x);  // -X
    if (face == 2) dir = float3(uv.x, 1.0, uv.y);    // +Y
    if (face == 3) dir = float3(uv.x, -1.0, -uv.y);  // -Y
    if (face == 4) dir = float3(uv.x, -uv.y, 1.0);   // +Z
    return normalize(dir);                           // -Z
}

float2 DirectionToEquirectangularUV(float3 dir)
{
    // Normalize direction
    dir = normalize(dir);

    // Convert 3D direction to UV coordinates
    float phi = atan2(dir.z, dir.x);
    float theta = asin(dir.y);

    float2 uv;
    uv.x = 0.5f + phi / (2.0f * PI); // Map to [0, 1]
    uv.y = 0.5f - theta / PI;       // Map to [0, 1]
    return uv;
}
#endif
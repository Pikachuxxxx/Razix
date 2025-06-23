#ifndef _LIGHT_HLSL_
#define _LIGHT_HLSL_
//----------------------------------------------------------------------------
// Constants and Defines
// Max no of lights in the scene
#define MAX_LIGHTS 128
//----------------------------------------------------------------------------
// Type of the Light (enum)
enum LightType : uint
{
    DIRECTIONAL = 0,
    POINT       = 1,
    SPOT        = 2,
    AREA,    // Not Supported!
    FLASH    // Not Supported!
};
//----------------------------------------------------------------------------
// Light Data
// The light info that every light stores in the scene
struct LightData
{
    float3 position;    // [point/spot] .xyz = position, .w = range
    float  range;
    float3 color;    // .rgb = color, .a = intensity
    float  intensity;
    float4 direction;    // [spot/directional] from light, normalized
    float  constantAttenuation;
    float  linearAttenuation;
    float  quadratic;
    float  innerConeAngle;    // [spot] in radians
    float  outerConeAngle;    // [spot] in radians
    uint   type;
};
//----------------------------------------------------------------------------
// GPU Light
cbuffer SceneLights : register(b0, space0)
{
    uint      numLights;
    LightData lightData[MAX_LIGHTS];
};
//----------------------------------------------------------------------------
#endif
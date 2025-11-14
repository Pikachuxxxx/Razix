#ifndef _LIGHT_DATA_H_
#define _LIGHT_DATA_H_

#define MAX_LIGHTS 128

enum LightType
{
    RZ_LIGHT_TYPE_DIRECTIONAL = 0,
    RZ_LIGHT_TYPE_POINT       = 1,
    RZ_LIGHT_TYPE_SPOT        = 2,
    RZ_LIGHT_TYPE_AREA,    // Not Supported!
    RZ_LIGHT_TYPE_FLASH    // Not Supported!
};

struct RAZIX_MEM_ALIGN_16 LightData
{
    alignas(16) float3 position        = float3(1.0f);
    alignas(4) f32 range               = 10.0f;
    alignas(16) float3 color           = float3(1.0f);
    alignas(4) f32 intensity           = 1.0f;
    alignas(16) float4 direction       = float4(1.0f);
    alignas(4) f32 constantAttenuation = 1.0f;
    alignas(4) f32 linearAttenuation   = 0.09f;
    alignas(4) f32 quadratic           = 0.032f;
    alignas(4) f32 innerConeAngle      = 12.5f;    // [Spot]
    alignas(4) f32 outerConeAngle      = 60.0f;    // [Spot]
    alignas(4) LightType type          = RZ_LIGHT_TYPE_DIRECTIONAL;
    alignas(4) u32 _padding[2]         = {0, 0};    // Implicit padding that will be consumed by GLSL for 16 byte alignment
};

typedef Razix::RZDynamicArray<LightData> LightsData;

#endif

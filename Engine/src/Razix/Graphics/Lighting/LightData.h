#ifndef _LIGHT_DATA_H_
#define _LIGHT_DATA_H_

#define MAX_LIGHTS 128

enum LightType : u32
{
    DIRECTIONAL = 0,
    Point       = 1,
    SPOT        = 2,
    AREA,    // Not Supported!
    FLASH    // Not Supported!
};

struct RAZIX_MEM_ALIGN LightData
{
    alignas(16) glm::vec3 position  = glm::vec3(1.0f);
    alignas(4) f32 range            = 10.0f;
    alignas(16) glm::vec3 color     = glm::vec3(1.0f);
    alignas(4) f32 intensity        = 1.0f;
    alignas(16) glm::vec4 direction = glm::vec4(1.0f);
    alignas(4) f32 constant         = 1.0f;
    alignas(4) f32 linear           = 0.09f;
    alignas(4) f32 quadratic        = 0.032f;
    alignas(4) f32 innerConeAngle   = 12.5f;    // [Spot]
    alignas(4) f32 outerConeAngle   = 60.0f;    // [Spot]
    alignas(4) LightType type       = LightType::DIRECTIONAL;
    alignas(4) u32 _padding[2]      = {0, 0};    // Implicit padding that will be consumed by GLSL for 16 byte alignment
};

#endif

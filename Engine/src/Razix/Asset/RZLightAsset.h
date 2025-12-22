#ifndef _RZ_LIGHT_ASSET_H_
#define _RZ_LIGHT_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

#define RAZIX_MAX_LIGHTS_PER_WORLD 1024

namespace Razix {

    typedef enum RZLightType
    {
        RZ_LIGHT_TYPE_DIRECTIONAL = 0,
        RZ_LIGHT_TYPE_POINT       = 1,
        RZ_LIGHT_TYPE_SPOT        = 2,
        RZ_LIGHT_TYPE_AREA,    // Not Supported!
        RZ_LIGHT_TYPE_FLASH    // Not Supported!
    } RZLightType;

    struct RAZIX_ALIGN_TO(RAZIX_16B_ALIGN) RZLightAsset
    {
        float4      position_Range;
        float4      color_Intensity;
        float4      direction;
        f32         constantAttenuation;
        f32         linearAttenuation;
        f32         quadratic;
        f32         innerConeAngle;
        f32         outerConeAngle;
        RZLightType type;
        u8          _pad0[8];
    };

    RAZIX_REFLECT_TYPE_START(RZLightAsset)
    RAZIX_REFLECT_MEMBER(position_Range)
    RAZIX_REFLECT_MEMBER(color_Intensity)
    RAZIX_REFLECT_MEMBER(direction)
    RAZIX_REFLECT_MEMBER(constantAttenuation)
    RAZIX_REFLECT_MEMBER(linearAttenuation)
    RAZIX_REFLECT_MEMBER(quadratic)
    RAZIX_REFLECT_MEMBER(innerConeAngle)
    RAZIX_REFLECT_MEMBER(outerConeAngle)
    RAZIX_REFLECT_MEMBER(type)
    RAZIX_REFLECT_TYPE_END(RZLightAsset)

}    // namespace Razix

#endif    // _RZ_LIGHT_ASSET_H_

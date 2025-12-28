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
        float4      Position_Range;
        float4      Color_Intensity;
        float4      Direction;
        f32         ConstantAttenuation;
        f32         LinearAttenuation;
        f32         Quadratic;
        f32         InnerConeAngle;
        f32         OuterConeAngle;
        RZLightType Type;
        u8          _pad0[8];
    };

    RAZIX_REFLECT_TYPE_START(RZLightAsset)
    RAZIX_REFLECT_MEMBER(Position_Range)
    RAZIX_REFLECT_MEMBER(Color_Intensity)
    RAZIX_REFLECT_MEMBER(Direction)
    RAZIX_REFLECT_MEMBER(ConstantAttenuation)
    RAZIX_REFLECT_MEMBER(LinearAttenuation)
    RAZIX_REFLECT_MEMBER(Quadratic)
    RAZIX_REFLECT_MEMBER(InnerConeAngle)
    RAZIX_REFLECT_MEMBER(OuterConeAngle)
    RAZIX_REFLECT_MEMBER(Type)
    RAZIX_REFLECT_TYPE_END(RZLightAsset)

}    // namespace Razix

#endif    // _RZ_LIGHT_ASSET_H_

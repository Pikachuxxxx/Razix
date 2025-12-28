#ifndef _RZ_PHYSICS_MATERIAL_ASSET_H_
#define _RZ_PHYSICS_MATERIAL_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

namespace Razix {

    // TODO: to be ratified later once physics engine integration is done (JOLT)
    struct RAZIX_ALIGN_TO(RAZIX_16B_ALIGN) RZPhysicsMaterialAsset
    {
        f32  Density;
        f32  StaticFriction;
        f32  DynamicFriction;
        f32  Restitution;
        f32  LinearDamping;
        f32  AngularDamping;
        bool bIsTrigger;
        u8   _pad0[7];
    };

    RAZIX_REFLECT_TYPE_START(RZPhysicsMaterialAsset)
    RAZIX_REFLECT_MEMBER(Density)
    RAZIX_REFLECT_MEMBER(StaticFriction)
    RAZIX_REFLECT_MEMBER(DynamicFriction)
    RAZIX_REFLECT_MEMBER(Restitution)
    RAZIX_REFLECT_MEMBER(LinearDamping)
    RAZIX_REFLECT_MEMBER(AngularDamping)
    RAZIX_REFLECT_MEMBER(bIsTrigger)
    RAZIX_REFLECT_TYPE_END(RZPhysicsMaterialAsset)

};    // namespace Razix

#endif    // _RZ_PHYSICS_MATERIAL_ASSET_H_

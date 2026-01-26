#ifndef _RZ_CLOTH_ASSET_H_
#define _RZ_CLOTH_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

#include "Razix/Core/RZHandle.h"

namespace Razix {

    struct RAZIX_ALIGN_TO(RAZIX_16B_ALIGN) RZClothAsset
    {
        RAZIX_ASSET

        u32       VertexCount;
        u32       TriangleCount;
        u32       ConstraintCount;
        f32       Mass;
        f32       Damping;
        f32       GravityScale;
        rz_handle MeshHandle;
        rz_handle PhysicsDataHandle;
        bool      bIsSimulating;
        u8        _pad0[7];
    };

    RAZIX_REFLECT_TYPE_START(RZClothAsset)
    RAZIX_REFLECT_ASSET_HEADER
    RAZIX_REFLECT_PRIMITIVE(VertexCount)
    RAZIX_REFLECT_PRIMITIVE(TriangleCount)
    RAZIX_REFLECT_PRIMITIVE(ConstraintCount)
    RAZIX_REFLECT_PRIMITIVE(Mass)
    RAZIX_REFLECT_PRIMITIVE(Damping)
    RAZIX_REFLECT_PRIMITIVE(GravityScale)
    RAZIX_REFLECT_PRIMITIVE(bIsSimulating)
    RAZIX_REFLECT_TYPE_END(RZClothAsset)

};    // namespace Razix

#endif    // _RZ_CLOTH_ASSET_H_

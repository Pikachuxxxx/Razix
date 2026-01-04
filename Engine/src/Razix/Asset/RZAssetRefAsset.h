#ifndef _RZ_ASSET_REF_ASSET_H_
#define _RZ_ASSET_REF_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

#include "Razix/Core/UUID/RZUUID.h"

namespace Razix {

    struct RZAssetRefAsset
    {
        RZUUID AssetUUID;
    };

    RAZIX_REFLECT_TYPE_START(RZAssetRefAsset)
    RAZIX_REFLECT_MEMBER(AssetUUID)
    RAZIX_REFLECT_TYPE_END(RZAssetRefAsset)

};    // namespace Razix

#endif    // _RZ_ASSET_REF_ASSET_H_

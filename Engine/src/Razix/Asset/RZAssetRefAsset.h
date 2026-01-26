#ifndef _RZ_ASSET_REF_ASSET_H_
#define _RZ_ASSET_REF_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

#include "Razix/Core/UUID/RZUUID.h"

namespace Razix {

    struct RZAssetRefAsset
    {
        RAZIX_ASSET

        RZUUID AssetUUID;
    };

    RAZIX_REFLECT_TYPE_START(RZAssetRefAsset)
    RAZIX_REFLECT_ASSET_HEADER
    RAZIX_REFLECT_UUID(AssetUUID)
    RAZIX_REFLECT_TYPE_END(RZAssetRefAsset)

};    // namespace Razix

#endif    // _RZ_ASSET_REF_ASSET_H_

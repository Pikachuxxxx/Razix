#pragma once

#include "Razix/AssetSystem/RZAssetBase.h"

namespace Razix {
    namespace AssetSystem {

        // [Static Checks]: Define C++ classes for each asset type registered for them to be instantiated
        //#define X(name) RAZIX_CHECK_IF_TYPE_IS_DEFINED(RZ##name##Asset, "[AssetSystem] Error: RZFooAsset class must be defined for AssetType::" #name "!")
        //        ASSET_TYPE_LIST
        //#undef X

        class RZAssetBuilder
        {};
    }    // namespace AssetSystem
}    // namespace Razix

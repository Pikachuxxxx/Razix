// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZAssetBase.h"

namespace Razix {
    namespace AssetSystem {

        RZAsset::RZAsset(const RZUUID& uuid, AssetType type)
            : m_UUID(uuid), m_Type(type)
        {
        }

    }    // namespace AssetSystem
}    // namespace Razix

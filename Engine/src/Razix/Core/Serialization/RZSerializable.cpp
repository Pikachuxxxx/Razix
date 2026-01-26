// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZSerializable.h"

#include "Razix/AssetSystem/RZAssetBase.h"

namespace Razix {

    void* RZAssetUtilCreateAssetInstanceInPlace(void* memoryBacking, void* coldDataBacking)
    {
        return reinterpret_cast<void*>(new (memoryBacking) Razix::RZAsset(Razix::RZAssetType::kTransform, coldDataBacking));
    }

}    // namespace Razix

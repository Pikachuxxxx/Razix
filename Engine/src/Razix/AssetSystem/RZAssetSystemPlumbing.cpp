// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZAssetBase.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Core/Memory/RZMemoryFunctions.h"

#include "Razix/AssetSystem/RZAssetDB.h"

#include "Razix/Gfx/Renderers/RZWorldRenderer.h"

namespace Razix {

    RAZIX_REGISTER_ASSET_POST_ASYNC_LOAD_CALLBACKS(MeshAsset, RZAssetType::kMesh, [](rz_asset_handle handle) {
            return;
            },
            [](rz_asset_handle handle) {
                RZMeshAsset* meshAsset = RZAssetDB::Get().getAssetResourceMutablePtr<RZMeshAsset>(handle);
                if (!meshAsset || !rz_handle_is_valid(&meshAsset->MeshHandle))
                    return;

                RZEngine::Get().getWorldRenderer().getMeshPool().deallocate(meshAsset->MeshHandle);
            });

}

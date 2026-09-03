#ifndef _RZ_MESH_ASSET_H_
#define _RZ_MESH_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include <Razix/Core/RZHandle.h>

#include "Razix/Core/Reflection/RZReflection.h"

namespace Razix {

    enum class MeshType
    {
        kStaticMesh,
        kDynamicMesh,
        kSkeletalMesh,
        kSMInstanced,
        kCOUNT
    };

    // TODO: To be merged with RZMesh class
    // TODO: Also RZMeshFactory will create these RZMeshAssets raw data without material data tied to it.
    struct RAZIX_ALIGN_TO(RAZIX_16B_ALIGN) RZMeshAsset
    {
        RAZIX_ASSET

        float3    BoundsMin;
        f32       BoundsRadius;
        float3    BoundsMax;
        u32       IndexCount;
        u32       VertexCount;
        MeshType  MeshType;
        rz_handle MeshHandle;
        u32       _pad0;
        RZString  MeshPath;
        RZString  MaterialPath;
    };

    RAZIX_REFLECT_TYPE_START(RZMeshAsset)
    RAZIX_REFLECT_ASSET_HEADER
    RAZIX_REFLECT_PRIMITIVE(VertexCount)
    RAZIX_REFLECT_PRIMITIVE(IndexCount)
    RAZIX_REFLECT_PRIMITIVE(BoundsMin)
    RAZIX_REFLECT_PRIMITIVE(BoundsRadius)
    RAZIX_REFLECT_PRIMITIVE(BoundsMax)
    RAZIX_REFLECT_PRIMITIVE(MeshType)
    RAZIX_REFLECT_STRING(MeshPath)
    RAZIX_REFLECT_STRING(MaterialPath)
    RAZIX_REFLECT_TYPE_END(RZMeshAsset)

};    // namespace Razix

#endif    // _RZ_MESH_ASSET_H_

#ifndef _RZ_MESH_ASSET_H_
#define _RZ_MESH_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

namespace Razix {

    // TODO: To be merged with RZMesh class
    struct RAZIX_ALIGN_TO(RAZIX_16B_ALIGN) RZMeshAsset
    {
        u32      VertexCount;
        u32      IndexCount;
        u32      VertexBufferHandle;
        u32      IndexBufferHandle;
        float3   BoundsMin;
        f32      BoundsRadius;
        float3   BoundsMax;
        u8       _pad0[4];
        RZString MeshPath;
        RZString MaterialPath;
    };

    RAZIX_REFLECT_TYPE_START(RZMeshAsset)
    RAZIX_REFLECT_MEMBER(VertexCount)
    RAZIX_REFLECT_MEMBER(IndexCount)
    RAZIX_REFLECT_MEMBER(VertexBufferHandle)
    RAZIX_REFLECT_MEMBER(IndexBufferHandle)
    RAZIX_REFLECT_MEMBER(BoundsMin)
    RAZIX_REFLECT_MEMBER(BoundsRadius)
    RAZIX_REFLECT_MEMBER(BoundsMax)
    RAZIX_REFLECT_MEMBER(MeshPath)
    RAZIX_REFLECT_MEMBER(MaterialPath)
    RAZIX_REFLECT_TYPE_END(RZMeshAsset)

};    // namespace Razix

#endif    // _RZ_MESH_ASSET_H_

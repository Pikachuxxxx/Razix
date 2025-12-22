#ifndef _RZ_TRANSFORM_ASSET_H_
#define _RZ_TRANSFORM_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

namespace Razix {

    /**
     * RZTransformAsset is a special asset, it stores all it's data in SOA form
     * It has 5 fields, Pos, Rotitionation, Scale, LocalTransformMatrix and WorldTransformMatrix.
     * Each of them will be in their own pool in a custom RZTransformAssetPool.
     * So we could store indices here but again, why not just re-use the rz_asset_handle? the lower 32 bits?
     *
     * TODO: If that seems to be an problem we can add another u32 to track all 5 fields arrays together.
     * 
     * Note: This struct is not the data itself. It's a reference/descriptor that says
     * "I am a Transform, and my data is stored in the Scene's Transform SoA pools."
     * RZTransformAsset is an exception as it's own data is stored in SOA formats in SceneGraph.
     */
    struct RZTransformAsset
    {
        RAZIX_ASSET;
        
    };

    // Define Serialization
    REFLECT_TYPE_START(RZTransformAsset)
    // Just the typical RZAsset header, which will be read as part of deserialization automatically.
    // The same index is used to laod data from SceneGraph, and load into the special RZTransformAssetPool 
    REFLECT_TYPE_END(RZTransformAsset)
};    // namespace Razix

#endif    // _RZ_TRANSFORM_ASSET_H_

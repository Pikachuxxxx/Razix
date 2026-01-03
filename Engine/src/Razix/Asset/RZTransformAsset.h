#ifndef _RZ_TRANSFORM_ASSET_H_
#define _RZ_TRANSFORM_ASSET_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
typedef struct rz_transform_flat
{
    float4 position;
    float4 rotation;    // Quaternion
    float4 scale;
    float4 _pad0; // Padding to make it 64 bytes, don't think about fetching next pos instead, this fitts well into AVX single register
} rz_transform_flat; // 4x4 = 64 bytes, single cache line load into AVX register, speeds up single ZMM operations

RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
typedef struct rz_transform_matrix
{
    float4x4 local;
    float4x4 world;
} rz_transform_matrix ; // 2x64 = 128 bytes, 2 cache line loads (AVX-512)

RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
typedef struct rz_transform_local_matrix
{
    float4x4 local;
} rz_transform_local_matrix; // 1 cache line load

RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
typedef struct rz_transform_world_matrix
{
    float4x4 world;
} rz_transform_world_matrix; // 1 cache line load

RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
typedef struct rz_transform
{
    rz_transform_flat   flat;
    rz_transform_matrix matrix;
} rz_transform; // 3 cache lines total

#ifdef __cplusplus

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
    RAZIX_REFLECT_TYPE_START(RZTransformAsset)
    // Just the typical RZAsset header, which will be read as part of deserialization automatically.
    // The same index is used to load data from SceneGraph, and load into the special RZTransformAssetPool
    // We will have a template specialization of RZAssetPool for RZTransformAsset to handle SOA pools
    // As for indexing, we can just use the lower 32-bits of the handle as the index into the SOA pools in SceneGraph, ultimate reuse
    // All other assets are stored in AOS format and can be saved to disk, but for transform asset the data is stored in SceneGraph file ex. SCN_TanuRoom.scene
    RAZIX_REFLECT_TYPE_END(RZTransformAsset)
};    // namespace Razix

#ifdef __cplusplus

#ifdef __cplusplus
}
#endif

#endif    // _RZ_TRANSFORM_ASSET_H_

#pragma once

#include "Razix/Core/RZDataTypes.h"
#include "Razix/Math/Math.h"

typedef struct rz_aabb
{
    float3 min;
    float _pad0;
    float3 max;
    float _pad1;
} rz_aabb;

RAZIX_API float3 rz_aabb_get_extent(const rz_aabb* aabb);
RAZIX_API float3 rz_aabb_get_center(const rz_aabb* aabb);
RAZIX_API f32    rz_aabb_get_radius(const rz_aabb* aabb);

RAZIX_API void rz_aabb_transform(rz_aabb* out_aabb, const rz_aabb* aabb, const float4x4* m);

// TODO: remove this BS!
#ifdef __cplusplus
namespace Razix {
    namespace Maths {
        typedef ::rz_aabb AABB;
    }
}    // namespace Razix
#endif

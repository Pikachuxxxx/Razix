#pragma once

#include "Razix/Core/RZDataTypes.h"
#include "Razix/Math/Math.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct rz_aabb
    {
        float3 min;
        float3 max;
    } rz_aabb;

    RAZIX_API float3 rz_aabb_get_extent(const rz_aabb* aabb);
    RAZIX_API float3 rz_aabb_get_center(const rz_aabb* aabb);
    RAZIX_API f32    rz_aabb_get_radius(const rz_aabb* aabb);

    RAZIX_API void rz_aabb_transform(rz_aabb* out_aabb, const rz_aabb* aabb, const float4x4* m);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace Razix {
    namespace Maths {
        typedef ::rz_aabb AABB;
    }
}    // namespace Razix
#endif

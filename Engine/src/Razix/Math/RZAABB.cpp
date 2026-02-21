// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZAABB.h"

RAZIX_API float3 rz_aabb_get_extent(const rz_aabb* aabb)
{
    return aabb->max - aabb->min;
}

RAZIX_API float3 rz_aabb_get_center(const rz_aabb* aabb)
{
    return (aabb->max + aabb->min) * 0.5f;
}

RAZIX_API f32 rz_aabb_get_radius(const rz_aabb* aabb)
{
    return glm::length(rz_aabb_get_extent(aabb) * 0.5f);
}

RAZIX_API void rz_aabb_transform(rz_aabb* out_aabb, const rz_aabb* aabb, const float4x4* m)
{
    // https://dev.theomader.com/transform-bounding-boxes/

    const auto xa = (*m)[0] * aabb->min.x;
    const auto xb = (*m)[0] * aabb->max.x;

    const auto ya = (*m)[1] * aabb->min.y;
    const auto yb = (*m)[1] * aabb->max.y;

    const auto za = (*m)[2] * aabb->min.z;
    const auto zb = (*m)[2] * aabb->max.z;

    out_aabb->min = glm::min(xa, xb) + glm::min(ya, yb) + glm::min(za, zb) + (*m)[3];
    out_aabb->max = glm::max(xa, xb) + glm::max(ya, yb) + glm::max(za, zb) + (*m)[3];
}

#pragma once

#include "Razix/AssetSystem/RZAssetFileSpec.h"

#include <glm/fwd.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <vector>

namespace Razix {
    namespace Gfx {

#define MAX_BONES_COUNT 4

#define SIMPLE_VERTEX_ATTRIBS_COUNT 0x2

        /**
         * Simple vertex encapsulates the minimum amount of data needed for rendering a simple mesh
         */
        struct RZSimpleVertex
        {
            float4 Position;
            float2 TexCoords;
        };

#define VERTEX_ATTRIBS_COUNT 0x5

#define VERTEX_ATTRIBS_POS_IDX 0x0
#define VERTEX_ATTRIBS_COL_IDX 0x1
#define VERTEX_ATTRIBS_UV_IDX  0x2
#define VERTEX_ATTRIBS_NOR_IDX 0x3
#define VERTEX_ATTRIBS_TAN_IDX 0x4

        /**
         * Razix Engine vertex data format that will be used to render complex 3D meshes and models 
         */
        struct RZVertex
        {
#if RAZIX_ASSET_VERSION == RAZIX_ASSET_VERSION_V1
            float3 Position;
            float4 Color;
            float2 UV;
            float3 Normal;
            float3 Tangent;

            RZVertex()
                : Position(float3(0.0f)), Color(float4(0.0f)), UV(float2(0.0f)), Normal(float3(0.0f)), Tangent(float3(0.0f)) {}

            bool operator==(const RZVertex& other) const
            {
                return Position == other.Position && UV == other.UV && Color == other.Color && Normal == other.Normal && Tangent == other.Tangent;
            }
#endif
            std::vector<float3> Position;
            std::vector<float4> Color;
            std::vector<float2> UV;
            std::vector<float3> Normal;
            std::vector<float3> Tangent;

            void setSize(uint32_t sz)
            {
                Position.resize(sz);
                Color.resize(sz);
                UV.resize(sz);
                Normal.resize(sz);
                Tangent.resize(sz);
            }
        };

#define SKELETL_VERTEX_ATTRIBS_COUNT 0x7

        /**
         * Skeletal vertex that will be used to render skinned meshes 
         */
        struct RZSkeletalVertex
        {
#if RAZIX_ASSET_VERSION == RAZIX_ASSET_VERSION_V1
            float3 Position;
            float2 UV;
            float3 Normal;
            float3 Tangent;
            float3 BiTangent;
            int4   BoneIndices; /* Indices of the bones that this vertex will be affected by, total bones = MAX_BONES_COUNT */
            float3 BoneWeights;
#endif
            std::vector<float3> Position;
            std::vector<float2> UV;
            std::vector<float3> Normal;
            std::vector<float3> Tangent;
            std::vector<float3> BiTangent;
            std::vector<int4>   BoneIndices;
            std::vector<float3> BoneWeights;

            void setSize(uint32_t sz)
            {
                Position.resize(sz);
                UV.resize(sz);
                Normal.resize(sz);
                Tangent.resize(sz);
                BiTangent.resize(sz);
                BoneIndices.resize(sz);
                BoneWeights.resize(sz);
            }
        };

    }    // namespace Gfx
}    // namespace Razix

#if RAZIX_ASSET_VERSION == RAZIX_ASSET_VERSION_V1
namespace std {
    template<>
    struct hash<Razix::Gfx::RZVertex>
    {
        sz operator()(Razix::Gfx::RZVertex const& vertex) const
        {
            return ((hash<float3>()(vertex.Position) ^ (hash<float2>()(vertex.UV) << 1) ^ (hash<float4>()(vertex.Color) << 1) ^ (hash<float3>()(vertex.Normal) << 1) ^ (hash<float3>()(vertex.Tangent) << 1)));
        }
    };
}    // namespace std
#endif
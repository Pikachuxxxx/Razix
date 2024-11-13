#pragma once

#include "Razix/AssetSystem/RZAssetFileSpec.h"

#include <glm/fwd.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <vector>

namespace Razix {
    namespace Graphics {

#define MAX_BONES_COUNT 4

#define SIMPLE_VERTEX_ATTRIBS_COUNT 0x2

        /**
         * Simple vertex encapsulates the minimum amount of data needed for rendering a simple mesh
         */
        struct RZSimpleVertex
        {
            glm::vec4 Position;
            glm::vec2 TexCoords;
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
            glm::vec3 Position;
            glm::vec4 Color;
            glm::vec2 UV;
            glm::vec3 Normal;
            glm::vec3 Tangent;

            RZVertex()
                : Position(glm::vec3(0.0f)), Color(glm::vec4(0.0f)), UV(glm::vec2(0.0f)), Normal(glm::vec3(0.0f)), Tangent(glm::vec3(0.0f)) {}

            bool operator==(const RZVertex& other) const
            {
                return Position == other.Position && UV == other.UV && Color == other.Color && Normal == other.Normal && Tangent == other.Tangent;
            }
#endif
            std::vector<glm::vec3> Position;
            std::vector<glm::vec4> Color;
            std::vector<glm::vec2> UV;
            std::vector<glm::vec3> Normal;
            std::vector<glm::vec3> Tangent;

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
            glm::vec3  Position;
            glm::vec2  UV;
            glm::vec3  Normal;
            glm::vec3  Tangent;
            glm::vec3  BiTangent;
            glm::ivec4 BoneIndices; /* Indices of the bones that this vertex will be affected by, total bones = MAX_BONES_COUNT */
            glm::vec3  BoneWeights;
#endif
            std::vector<glm::vec3>  Position;
            std::vector<glm::vec2>  UV;
            std::vector<glm::vec3>  Normal;
            std::vector<glm::vec3>  Tangent;
            std::vector<glm::vec3>  BiTangent;
            std::vector<glm::ivec4> BoneIndices;
            std::vector<glm::vec3>  BoneWeights;

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

    }    // namespace Graphics
}    // namespace Razix

#if RAZIX_ASSET_VERSION == RAZIX_ASSET_VERSION_V1
namespace std {
    template<>
    struct hash<Razix::Graphics::RZVertex>
    {
        sz operator()(Razix::Graphics::RZVertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.Position) ^ (hash<glm::vec2>()(vertex.UV) << 1) ^ (hash<glm::vec4>()(vertex.Color) << 1) ^ (hash<glm::vec3>()(vertex.Normal) << 1) ^ (hash<glm::vec3>()(vertex.Tangent) << 1)));
        }
    };
}    // namespace std
#endif
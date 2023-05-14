#pragma once

#include <glm/fwd.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace Razix {
    namespace Graphics {

#define MAX_BONES_COUNT 4

        /**
         * Simple vertex encapsulates the minimum amount of data needed for rendering a simple mesh
         */
        struct RZSimpleVertex
        {
            glm::vec4 Position;
            glm::vec2 TexCoords;
        };

        /**
         * Razix Engine vertex data format that will be used to render complex 3D meshes and models 
         */
        struct RZVertex
        {
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
        };

        /**
         * Skeletal vertex that will be used to render skinned meshes 
         */
        struct RZSkeletalVertex
        {
            glm::vec3  Position;
            glm::vec2  UV;
            glm::vec3  Normal;
            glm::vec3  Tangent;
            glm::vec3  BiTangent;
            glm::ivec4 BoneIndices; /* Indices of the bones that this vertex will be affected by, total bones = MAX_BONES_COUNT */
            glm::vec3  BoneWeights;
        };

    }    // namespace Graphics
}    // namespace Razix
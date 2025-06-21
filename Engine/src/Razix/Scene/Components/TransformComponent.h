#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include <cereal/cereal.hpp>

#include "Razix/Core/Reflection/RZReflection.h"

namespace Razix {
    /**
     * A transform components represents the transformation of the entity in the game world
     */
    struct RAZIX_API TransformComponent
    {
        float3   Translation = {0.0f, 0.0f, 0.0f};
        float3   Rotation    = {0, 0, 0}; /* Stores the Rotation in radians */
        float3   Scale       = {1.0f, 1.0f, 1.0f};
        float4x4 Transform   = float4x4(1.0);
        // TODO: Serialize this?
        float4x4 WorldMatrix = float4x4(1.0);

        TransformComponent()                          = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const float3& translation)
            : Translation(translation) {}

        /* Gets the transformation matrix */
        float4x4 GetGlobalTransform();
        float4x4 GetLocalTransform();
        float4x4 GetWorldTransform() { return GetGlobalTransform(); }
        void     SetWorldTransform(float4x4 matrix) { WorldMatrix = matrix; }

        template<typename Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Translation", Translation), cereal::make_nvp("Rotation", Rotation), cereal::make_nvp("Scale", Scale), cereal::make_nvp("Transform", Transform));
        }
    };

    REFLECT_TYPE_START(TransformComponent)
    REFLECT_MEMBER(Translation)
    REFLECT_MEMBER(Rotation)
    REFLECT_MEMBER(Scale)
    REFLECT_TYPE_END(TransformComponent)

}    // namespace Razix
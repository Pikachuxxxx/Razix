// clang-format off
#include "rzxpch.h"
// clang-format on
#include "TransformComponent.h"

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

// https://stackoverflow.com/questions/49609654/quaternion-based-first-person-view-camera

float4x4 Razix::TransformComponent::GetGlobalTransform()
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

#if 0
quat qPitch = angleAxis(Rotation.x, float3(1, 0, 0));
    quat qYaw   = angleAxis(Rotation.y, gl m::vec3(0, 1, 0));
    quat qRoll  = angleAxis(Rotation.z, float3(0, 0, 1));

    quat orientation = qPitch * qYaw;
    orientation           = normalize(orientation);

    model = translate(float4x4(1.0f), Translation);
    model *= eulerAngleYZX(Rotation.y, Rotation.x, Rotation.z);
    model *= scale(float4x4(1.0f), Scale);
#endif

    Transform = float4x4(1.0f);

    quat     rotationQuat{};
    float4x4 rotationMatrix = float4x4(1.0f);
    // Convert the Euler angles into
    rotationQuat   = quat(Rotation);
    rotationMatrix = mat4_cast(rotationQuat);

    Transform = translate(Transform, Translation);
    Transform *= rotationMatrix;
    Transform = scale(Transform, Scale);

    return WorldMatrix * Transform;
}

float4x4 Razix::TransformComponent::GetLocalTransform()
{
    Transform = float4x4(1.0f);

    quat     rotationQuat{};
    float4x4 rotationMatrix = float4x4(1.0f);
    // Convert the Euler angles into
    rotationQuat   = quat(Rotation);
    rotationMatrix = mat4_cast(rotationQuat);

    Transform = translate(Transform, Translation);
    Transform *= rotationMatrix;
    Transform = scale(Transform, Scale);

    return Transform;
}
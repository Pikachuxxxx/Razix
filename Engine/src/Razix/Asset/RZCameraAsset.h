#ifndef _RZ_CAMERA_ASSET_H_
#define _RZ_CAMERA_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

#include "Razix/Gfx/Cameras/RZCamera3D.h"

namespace Razix {

    // TODO: instead of duplicating, we can instead store RZCameraAsset inside RZCamera3D for public/serialization and keep private separate?
    struct RAZIX_ALIGN_TO(RAZIX_16B_ALIGN) RZCameraAsset
    {
        float3                     Position;
        f32                        Yaw;
        float3                     Front;
        f32                        Pitch;
        float3                     Up;
        f32                        MovementSpeed;
        float3                     Right;
        f32                        MouseSensitivity;
        float3                     WorldUp;
        f32                        Zoom;
        f32                        DampingFactor;
        f32                        AspectRatio;
        f32                        PerspectiveFOV;
        f32                        PerspectiveNear;
        f32                        PerspectiveFar;
        f32                        OrthographicSize;
        f32                        OrthographicNear;
        f32                        OrthographicFar;
        RZCamera3D::ProjectionType ProjectionType;
        u8                         _pad0[12];
    };

    RAZIX_REFLECT_TYPE_START(RZCameraAsset)
    RAZIX_REFLECT_MEMBER(Position)
    RAZIX_REFLECT_MEMBER(Yaw)
    RAZIX_REFLECT_MEMBER(Front)
    RAZIX_REFLECT_MEMBER(Pitch)
    RAZIX_REFLECT_MEMBER(Up)
    RAZIX_REFLECT_MEMBER(MovementSpeed)
    RAZIX_REFLECT_MEMBER(Right)
    RAZIX_REFLECT_MEMBER(MouseSensitivity)
    RAZIX_REFLECT_MEMBER(WorldUp)
    RAZIX_REFLECT_MEMBER(Zoom)
    RAZIX_REFLECT_MEMBER(DampingFactor)
    RAZIX_REFLECT_MEMBER(AspectRatio)
    RAZIX_REFLECT_MEMBER(PerspectiveFOV)
    RAZIX_REFLECT_MEMBER(PerspectiveNear)
    RAZIX_REFLECT_MEMBER(PerspectiveFar)
    RAZIX_REFLECT_MEMBER(OrthographicSize)
    RAZIX_REFLECT_MEMBER(OrthographicNear)
    RAZIX_REFLECT_MEMBER(OrthographicFar)
    RAZIX_REFLECT_MEMBER(ProjectionType)
    RAZIX_REFLECT_TYPE_END(RZCameraAsset)

};    // namespace Razix

#endif    // _RZ_CAMERA_ASSET_H_

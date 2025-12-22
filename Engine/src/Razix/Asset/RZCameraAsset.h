#ifndef _RZ_CAMERA_ASSET_H_
#define _RZ_CAMERA_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

#include "Razix/Gfx/Cameras/RZCamera3D.h"

namespace Razix {

    struct RZCameraAsset
    {
        RAZIX_ASSET;
        RZCamera3D camera;
    };

    REFLECT_TYPE_START(RZCameraAsset)
    REFLECT_TYPE_END(RZCameraAsset)
};    // namespace Razix

#endif    // _RZ_CAMERA_ASSET_H_

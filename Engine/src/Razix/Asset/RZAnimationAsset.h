#ifndef _RZ_ANIMATION_ASSET_H_
#define _RZ_ANIMATION_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

#include "Razix/Core/RZHandle.h"

namespace Razix {

    struct RAZIX_ALIGN_TO(RAZIX_16B_ALIGN) RZAnimationAsset
    {
        u32       FrameCount;
        u32       BoneCount;
        f32       Duration;
        f32       FrameRate;
        rz_handle SkeletonHandle;
        rz_handle AnimationDataHandle;
        bool      bShouldLoop;
        u8        _pad0[7];
    };

    RAZIX_REFLECT_TYPE_START(RZAnimationAsset)
    RAZIX_REFLECT_MEMBER(FrameCount)
    RAZIX_REFLECT_MEMBER(BoneCount)
    RAZIX_REFLECT_MEMBER(Duration)
    RAZIX_REFLECT_MEMBER(FrameRate)
    RAZIX_REFLECT_MEMBER(bShouldLoop)
    RAZIX_REFLECT_TYPE_END(RZAnimationAsset)

};    // namespace Razix

#endif    // _RZ_ANIMATION_ASSET_H_

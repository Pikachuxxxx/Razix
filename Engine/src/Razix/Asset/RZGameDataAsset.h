#ifndef _RZ_GAME_DATA_ASSET_H_
#define _RZ_GAME_DATA_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

#include "Razix/Core/Containers/string.h"

namespace Razix {

    struct RZGameDataAsset
    {
        u32      SaveSlotCount;
        u32      TotalPlayTime;
        u32      DifficultyLevel;
        bool     bIsDLCEnabled;
        u8       _pad0[3];
        float3   CheckpointPosition;
        u8       _pad1[4];
        RZString LastPlayedLevel;
    };

    RAZIX_REFLECT_TYPE_START(RZGameDataAsset)
    RAZIX_REFLECT_MEMBER(SaveSlotCount)
    RAZIX_REFLECT_MEMBER(TotalPlayTime)
    RAZIX_REFLECT_MEMBER(bIsDLCEnabled)
    RAZIX_REFLECT_MEMBER(DifficultyLevel)
    RAZIX_REFLECT_MEMBER(LastPlayedLevel)
    RAZIX_REFLECT_MEMBER(CheckpointPosition)
    RAZIX_REFLECT_TYPE_END(RZGameDataAsset)

};    // namespace Razix

#endif    // _RZ_GAME_DATA_ASSET_H_

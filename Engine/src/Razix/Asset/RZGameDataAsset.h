#ifndef _RZ_GAME_DATA_ASSET_H_
#define _RZ_GAME_DATA_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

#include "Razix/Core/Containers/string.h"

namespace Razix {

    struct RZGameDataAsset
    {
        RAZIX_ASSET

        u32      SaveSlotCount;
        u32      TotalPlayTime;
        u32      DifficultyLevel;
        bool     bIsDLCEnabled;
        u8       _pad0[3];
        float3   CheckpointPosition;
        u32      _pad1;
        RZString LastPlayedLevel;
    };

    RAZIX_REFLECT_TYPE_START(RZGameDataAsset)
    RAZIX_REFLECT_ASSET_HEADER
    RAZIX_REFLECT_PRIMITIVE(SaveSlotCount)
    RAZIX_REFLECT_PRIMITIVE(TotalPlayTime)
    RAZIX_REFLECT_PRIMITIVE(bIsDLCEnabled)
    RAZIX_REFLECT_PRIMITIVE(DifficultyLevel)
    RAZIX_REFLECT_PRIMITIVE(LastPlayedLevel)
    RAZIX_REFLECT_PRIMITIVE(CheckpointPosition)
    RAZIX_REFLECT_STRING(LastPlayedLevel)
    RAZIX_REFLECT_TYPE_END(RZGameDataAsset)

};    // namespace Razix

#endif    // _RZ_GAME_DATA_ASSET_H_

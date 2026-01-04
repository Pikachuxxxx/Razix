#ifndef _RZ_LOVE_PUZZLE_ASSET_H_
#define _RZ_LOVE_PUZZLE_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

#include "Razix/Core/Containers/string.h"

namespace Razix {

    struct RZLovePuzzleAsset
    {
        RZString Question;
        RZString Answer;
        u32      HintCount;
        u32      Difficulty;
        bool     IsSolved;
        u8       _pad0[7];
    };

    RAZIX_REFLECT_TYPE_START(RZLovePuzzleAsset)
    RAZIX_REFLECT_MEMBER(Question)
    RAZIX_REFLECT_MEMBER(Answer)
    RAZIX_REFLECT_MEMBER(HintCount)
    RAZIX_REFLECT_MEMBER(Difficulty)
    RAZIX_REFLECT_MEMBER(IsSolved)
    RAZIX_REFLECT_TYPE_END(RZLovePuzzleAsset)

};    // namespace Razix

#endif    // _RZ_LOVE_PUZZLE_ASSET_H_

#ifndef _RZ_VIGNERE_PUZZLE_ASSET_H_
#define _RZ_VIGNERE_PUZZLE_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

#include "Razix/Core/Containers/string.h"

namespace Razix {

    struct RZVigneerePuzzleAsset
    {
        RZString PlainText;
        RZString CipherText;
        RZString Key;
        u32      Difficulty;
        bool     IsSolved;
        u8       _pad0[7];
    };

    RAZIX_REFLECT_TYPE_START(RZVigneerePuzzleAsset)
    RAZIX_REFLECT_MEMBER(PlainText)
    RAZIX_REFLECT_MEMBER(CipherText)
    RAZIX_REFLECT_MEMBER(Key)
    RAZIX_REFLECT_MEMBER(Difficulty)
    RAZIX_REFLECT_TYPE_END(RZVigneerePuzzleAsset)

};    // namespace Razix

#endif    // _RZ_VIGNERE_PUZZLE_ASSET_H_

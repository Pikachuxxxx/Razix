#ifndef _RZ_VIGNERE_PUZZLE_ASSET_H_
#define _RZ_VIGNERE_PUZZLE_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Reflection/RZReflection.h"

#include "Razix/Core/Containers/string.h"

// TODO: Cryptography/RZVignereCipher.h/c

namespace Razix {

    struct RZVignerePuzzleAsset
    {
        // TODO: Replace with RZASCIIString for ASCII only chars
        RZString PlainText;
        RZString Key;
        // TODO: Replace it with a rz_bitset type (maybe C++ for calling functions on it)
        bool bIsSolved;
        u8   _pad0[7];
    };

    RAZIX_REFLECT_TYPE_START(RZVignerePuzzleAsset)
    RAZIX_REFLECT_STRING(PlainText)
    RAZIX_REFLECT_STRING(Key)
    RAZIX_REFLECT_PRIMITIVE(bIsSolved)
    RAZIX_REFLECT_TYPE_END(RZVignerePuzzleAsset)

};    // namespace Razix

#endif    // _RZ_VIGNERE_PUZZLE_ASSET_H_

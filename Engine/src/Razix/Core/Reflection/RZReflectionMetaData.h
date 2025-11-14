#pragma once

#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Containers/hash_map.h"
#include "Razix/Core/Containers/string.h"

namespace Razix {

    struct MemberMetaData
    {
        RZString name;
        RZString typeName;
        u32      offset;
        u32      size;
    };

    struct TypeMetaData
    {
        RZString                       name;
        RZString                       typeName;
        u32                            size;
        RZDynamicArray<MemberMetaData> members;
    };

    /**
     * Assuming every enum in the Razix code bases uses a COUNT member at the end
     */
    struct EnumMetaData
    {
        RZString                              name;
        RZString                              typeName;
        RZDynamicArray<RZPair<RZString, int>> values;
    };

    struct BitFieldMetaData
    {
        RZString                              name;
        RZString                              typeName;
        RZDynamicArray<RZPair<RZString, int>> values;    // int describes the bit index
    };
}    // namespace Razix
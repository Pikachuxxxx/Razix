#pragma once

#include <string>
#include <vector>

#include "Razix/Core/RZDataTypes.h"

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
        RZString                    name;
        RZString                    typeName;
        u32                         size;
        std::vector<MemberMetaData> members;
    };

    /**
     * Assuming every enum in the Razix code bases uses a COUNT member at the end
     */
    struct EnumMetaData
    {
        RZString                              name;
        RZString                              typeName;
        std::vector<std::pair<RZString, int>> values;
    };

    struct BitFieldMetaData
    {
        RZString                              name;
        RZString                              typeName;
        std::vector<std::pair<RZString, int>> values;    // int describes the bit index
    };
}    // namespace Razix
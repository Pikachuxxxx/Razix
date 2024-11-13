#pragma once

#include <string>
#include <vector>

#include "Razix/Core/RZDataTypes.h"

namespace Razix {

    struct MemberMetaData
    {
        std::string name;
        std::string typeName;
        u32         offset;
        u32         size;
    };

    struct TypeMetaData
    {
        std::string                 name;
        std::string                 typeName;
        u32                         size;
        std::vector<MemberMetaData> members;
    };

    /**
     * Assuming every enum in the Razix code bases uses a COUNT member at the end
     */
    struct EnumMetaData
    {
        std::string                              name;
        std::string                              typeName;
        std::vector<std::pair<std::string, int>> values;
    };

    struct BitFieldMetaData
    {
        std::string                              name;
        std::string                              typeName;
        std::vector<std::pair<std::string, int>> values;    // int describes the bit index
    };
}    // namespace Razix
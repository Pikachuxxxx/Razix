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
       
}    // namespace Razix
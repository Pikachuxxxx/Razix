#pragma once

#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Containers/hash_map.h"
#include "Razix/Core/Containers/string.h"

namespace Razix {

    enum class SerializeableDataType : u8
    {
        kPrimitive,
        kBlob,
        kArray,
        kHashMap,
        kString,
        kObject,    // POD struct
        kObjectArray,
        kEnum,
        kBitField,
        COUNT
    };

    struct MemberMetaData
    {
        RZString              name;     // variable name
        RZString              typeName; // underlying typename
        u32                   offset;
        u32                   size;
        SerializeableDataType dataType; // filled by user reflection macros
        // some additional flags to hint serilization behaviour, these are set by the user reflection macros
        union
        {
            u32 flags;
            struct
            {
                u32 isTriviallySerializable: 1;
                u32 forceCompression: 1;
                u32 compressionMethod: 2;    // 0: none, 1: zlib, 2: lz4, 3: custom
                u32 reserved : 30;
            };
        };
    };

    struct TypeMetaData
    {
        RZString                       name; // variable name
        RZString                       typeName; // underlying typename, actaully same as name for root TypeMetaData?
        u32                            size;
        RZDynamicArray<MemberMetaData> members;
    };
}    // namespace Razix

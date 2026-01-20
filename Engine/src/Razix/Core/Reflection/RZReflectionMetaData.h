#ifndef _RZ_REFLECTION_METADATA_H_
#define _RZ_REFLECTION_METADATA_H_

#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Compression/RZCompression.h"

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
        RZString              name;        // variable name
        RZString              typeName;    // underlying typename
        u32                   offset;
        u32                   size;        // TODO: use fixed sizes instead of sizeof for compatibility across platforms
        SerializeableDataType dataType;    // filled by user reflection macros
        // some additional flags to hint serilization behaviour, these are set by the user reflection macros
        union
        {
            u32 flags;
            struct
            {
                u32                 isTriviallySerializable : 1;    // seems redundant with TypeMetaData::bIsTriviallySerializable?
                u32                 forceCompression : 1;
                rz_compression_type compressionMethod : 3;
                u32                 reserved : 27;
            };
        };
    };

    struct TypeMetaData
    {
        RZString                       name;        // variable name
        RZString                       typeName;    // underlying typename, actaully same as name for root TypeMetaData?
        u32                            size;        // not actual size but sizeof for the type, not serialized size
        RZDynamicArray<MemberMetaData> members;
        bool                           bIsTriviallySerializable = true;    // if true, the whole struct can be serialized as a blob
    };
}    // namespace Razix

#endif    // _RZ_REFLECTION_METADATA_H_

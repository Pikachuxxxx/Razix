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

    struct ArrayOps
    {
        const void* (*get_data)(const void*);
        size_t (*get_size)(const void*);
        void (*set_data)(void*, const void*);
        void (*set_size)(void*, size_t);
    };

    template<typename ArrayT>
    constexpr ArrayOps make_array_ops()
    {
        return {
            +[](const void* arr) -> const void* {
                return static_cast<const ArrayT*>(arr)->data();
            },
            +[](const void* arr) -> size_t {
                return static_cast<const ArrayT*>(arr)->size();
            },
            +[](void* arr, const void* data) {
                static_cast<ArrayT*>(arr)->set_data(static_cast<const typename ArrayT::value_type*>(data));
            },
            +[](void* arr, size_t size) {
                static_cast<ArrayT*>(arr)->set_size(size);
            }};
    }

    // TODO: Add HashMap ops and other complex types ops as needed

    struct HashMapOps
    {
        const void* (*get_keys)(const void*);
        const void* (*get_values)(const void*);
        size_t (*get_size)(const void*);
        void (*set_data)(void*, const void*, const void*);
        void (*set_size)(void*, size_t);
    };

    template<typename MapT>
    constexpr HashMapOps make_hashmap_ops()
    {
        return {
            +[](const void* map) -> const void* {
                return static_cast<const MapT*>(map)->keys();
            },
            +[](const void* map) -> const void* {
                return static_cast<const MapT*>(map)->values();
            },
            +[](const void* map) -> size_t {
                RAZIX_CORE_TRACE("Getting size of HashMap");
                u32 value = static_cast<const MapT*>(map)->size();
                RAZIX_CORE_TRACE("Getting size of HashMap: {}", value);
                return value;
            },
            +[](void* map, const void* keys, const void* values) {
                static_cast<MapT*>(map)->insert_multiple(static_cast<const typename MapT::key_type*>(keys),
                    static_cast<const typename MapT::value_type*>(values),
                    static_cast<const size_t>(static_cast<const MapT*>(map)->size()));
            },
            +[](void* map, size_t size) {
                static_cast<MapT*>(map)->set_size(size);
            }};
    }

    struct MemberMetaData
    {
        RZString              name;        // variable name
        RZString              typeName;    // underlying typename
        u32                   offset;      // offset in the parent struct
        SerializeableDataType dataType;    // filled by user reflection macros
        // some additional flags to hint serilization behaviour, these are set by the user reflection macros
        union
        {
            u32 flags;
            struct
            {
                u32                 isStaticCompileSizedFixed : 1;    // seems redundant with TypeMetaData::bIsTriviallySerializable?
                u32                 forceCompression : 1;
                rz_compression_type compressionMethod : 3;
                u32                 reserved : 27;
            };
        };

        union
        {
            struct
            {
                u32 size;    // sizeof(member)
            } trivial;       // used for primitive and blob types

            struct
            {
                u32      elementSize;
                u32      elementCount;
                ArrayOps ops;
            } array;    // used for dynamic array types

            struct
            {
                ArrayOps ops;
            } string;    // used for string types

            struct
            {
                u32        keySize;
                u32        valueSize;
                HashMapOps ops;
            } map;

            struct
            {
                std::type_index type;
            } object;    // nested object type info, use this to get TypeMetaData from the registry and call processMember recursively on it's members
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

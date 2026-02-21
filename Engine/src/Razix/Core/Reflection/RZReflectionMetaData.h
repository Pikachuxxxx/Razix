#ifndef _RZ_REFLECTION_METADATA_H_
#define _RZ_REFLECTION_METADATA_H_

#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Compression/RZCompression.h"

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Containers/hash_map.h"
#include "Razix/Core/Containers/string.h"

#include "Razix/Core/UUID/RZUUID.h"

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
        kUUID,
        COUNT
    };

    struct ArrayOps
    {
        const void* (*get_data)(const void*);
        size_t (*get_size)(const void*);
        void (*set_data)(void*, const void*);
        void (*set_size)(void*, size_t);
    };

    using StringOps = ArrayOps;

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
        void (*get_data)(const void*, void*);
        size_t (*get_size)(const void*);
        void (*set_data)(void*, const void*);
        void (*set_size)(void*, size_t);
    };

    template<typename MapT>
    constexpr HashMapOps make_hashmap_ops()
    {
        return {
            +[](const void* map, void* buffer) -> void {
                return static_cast<const MapT*>(map)->write_key_values(buffer);
            },
            +[](const void* map) -> size_t {
                return static_cast<const MapT*>(map)->size();
            },
            +[](void* map, const void* keys_values) {
                static_cast<MapT*>(map)->insert_multiple(keys_values, static_cast<const size_t>(static_cast<const MapT*>(map)->size()));
            },
            +[](void* map, size_t size) {
                static_cast<MapT*>(map)->set_size(size);
            }};
    }

    struct UUIDOps
    {
        const void* (*get_data)(const rz_uuid*);
        void (*set_data)(rz_uuid*, const void*);
    };

    constexpr UUIDOps make_uuid_ops()
    {
        return {
            +[](const rz_uuid* uuid) -> const void* {
                return uuid->data;
            },
            +[](rz_uuid* uuid, const void* data) {
                memcpy(uuid->data, data, 16);
            }};
    }

    struct TypeMetaOps
    {
        ArrayOps   arrayOps;
        StringOps  stringOps;
        HashMapOps hashMapOps;
        UUIDOps    uuidOps;
    };

    union TypeMetaAccess
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
            u32             keySize;
            u32             valueSize;
            std::type_index keyType;
            std::type_index valueType;
            HashMapOps      ops;
        } map;

        struct
        {
            std::type_index type;
        } object;    // nested object type info, use this to get TypeMetaData from the registry and call processMember recursively on it's members

        struct
        {
            UUIDOps ops;
        } uuid;
    };

    struct MemberMetaData
    {
        RZString              name;        // variable name
        RZString              typeName;    // underlying typename
        u32                   offset;      // offset in the parent struct
        SerializeableDataType dataType;    // filled by user reflection macros
        // some additional flags to hint sterilization behaviour, these are set by the user reflection macros
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

        // Replace with TypeMetaAccess, I don't want to refactor so duplicating it for now
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
                StringOps ops;
            } string;    // used for string types

            struct
            {
                u32             keySize;
                u32             valueSize;
                std::type_index keyType;
                std::type_index valueType;
                HashMapOps      ops;
            } map;

            struct
            {
                std::type_index type;
            } object;    // nested object type info, use this to get TypeMetaData from the registry and call processMember recursively on it's members

            struct
            {
                UUIDOps ops;
            } uuid;
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

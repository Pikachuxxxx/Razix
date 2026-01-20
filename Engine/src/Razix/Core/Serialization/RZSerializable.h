#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include "Razix/Core/std/type_traits.h"
#include <Core/Reflection/RZReflectionMetaData.h>

namespace Razix {

    enum class DiskTypeTag : u8
    {
        Primitive   = 1,
        Blob        = 2,
        Array       = 3,
        HashMap     = 4,
        String      = 5,
        Object      = 6,
        ObjectArray = 7,
        Enum        = 8,
        BitField    = 9,
    };

    DiskTypeTag ToDiskTag(SerializeableDataType t)
    {
        switch (t) {
            default: /* error */;
            case SerializeableDataType::kPrimitive: return DiskTypeTag::Primitive;
            case SerializeableDataType::kBlob: return DiskTypeTag::Blob;
            case SerializeableDataType::kArray: return DiskTypeTag::Array;
            case SerializeableDataType::kHashMap: return DiskTypeTag::HashMap;
            case SerializeableDataType::kString: return DiskTypeTag::String;
            case SerializeableDataType::kObject: return DiskTypeTag::Object;
            case SerializeableDataType::kObjectArray: return DiskTypeTag::ObjectArray;
            case SerializeableDataType::kEnum: return DiskTypeTag::Enum;
            case SerializeableDataType::kBitField: return DiskTypeTag::BitField;
        }
        return DiskTypeTag::Primitive;

    }

    //-------------------------------------------------------------------------
    //  Serialized Data types
    //-------------------------------------------------------------------------

    struct SerializedPrimitive
    {
        union
        {
            u8   u8Value;
            i8   i8Value;
            u16  u16Value;
            i16  i16Value;
            u32  u32Value;
            i32  i32Value;
            u64  u64Value;
            i64  i64Value;
            f32  f32Value;
            f64  f64Value;
            bool boolValue;
        };
    };

    struct SerializedBlob
    {
        u32 offset;
        u32 size;
        u32 type_hash;
        u8  compression;
        u8  reserved[3];
        u32 decompressed_size;
    };

    // Array of anything
    struct SerializedArray
    {
        SerializedBlob data;    // blob of all elements
        u32            element_count;
        u32            element_type_hash;
        u8             element_size;
        u8             reserved[3];
    };

    // HashMap
    struct SerializedHashMap
    {
        SerializedBlob keys;      // blob of all keys
        SerializedBlob values;    // blob of all values
        u32            capacity;
        u32            count;
        u32            index;
    };

    // String
    struct SerializedString
    {
        SerializedBlob data;    // blob of characters
        u32            length;
        u8             encoding;    // UTF-8, UTF-16, ASCII
        u8             reserved[3];
    };

    // Struct instance - only works for POD/simple structs without pointers. Marked as clean POD in reflection registry.
    struct SerializedObject
    {
        SerializedBlob data;    // blob of struct bytes
        u32            type_hash;
        u32            size;
    };

    // Array of objects - only works for POD/simple structs without pointers. Marked as clean POD in reflection registry.
    struct SerializedObjectArray
    {
        SerializedBlob data;    // blob containing array of blobs
        u32            element_count;
        u32            element_type_hash;
    };

    struct SerialzeEnumValue
    {
        RZString name;
        int      value;
    };

    struct SerializeBitFieldValue
    {
        RZString name;
        int      bitIndex;
    };

    using RZSerialzableVariant = std::variant<
        SerializedPrimitive,
        SerializedBlob,
        SerializedArray,
        SerializedHashMap,
        SerializedString,
        SerializedObject,
        SerializedObjectArray,
        SerialzeEnumValue,
        SerializeBitFieldValue>;

    template<typename Derived>
    class RZSerializable
    {
    public:
        virtual ~RZSerializable() = default;

        static const TypeMetaData* getTypeMetaData()
        {
            return Razix::RZTypeRegistry::getTypeMetaData<rz_remove_cv_t<rz_remove_pointer_t<Derived>>>();
        }

        static RZDynamicArray<u8> serializeToBinary(const Derived& data)
        {
            RZDynamicArray<u8>  buffer;
            const TypeMetaData* meta = getTypeMetaData();
            if (!meta) return buffer;

            const u8* base = reinterpret_cast<const u8*>(&data);

            for (const auto& member: meta->members) {
                size_t oldSize = buffer.size();
                buffer.resize(oldSize + member.size);

                printf("member.name:  | member size: %d \n", member.size);

                memcpy(buffer.data() + oldSize,
                    base + member.offset,
                    member.size);
            }

            return buffer;
        }

        static Derived deserializeFromBinary(const RZDynamicArray<u8>& binary)
        {
            Derived data = {};

            const TypeMetaData* meta = getTypeMetaData();
            if (!meta) return data;

            u8*    base   = reinterpret_cast<u8*>(&data);
            size_t offset = 0;

            for (const auto& member: meta->members) {
                if (offset + member.size > binary.size()) break;
                memcpy(base + member.offset, binary.data() + offset, member.size);
                offset += member.size;
            }

            return data;
        }
    };
}    // namespace Razix
#endif    // SERIALZABLE_H

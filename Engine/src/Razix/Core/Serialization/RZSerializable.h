#ifndef _RZ_SERIALIZABLE_H
#define _RZ_SERIALIZABLE_H

#include "Razix/Core/std/type_traits.h"
#include "Razix/Core/std/utility.h"

#include "Razix/Core/Compression/RZCompression.h"

#include "Razix/Core/Reflection/RZReflectionMetaData.h"

namespace Razix {

    enum class RZDiskTypeTag : u8
    {
        kPrimitive   = 1,
        kBlob        = 2,
        kArray       = 3,
        kHashMap     = 4,
        kString      = 5,
        kObject      = 6,
        kObjectArray = 7,
        kEnum        = 8,
        kBitField    = 9,
        COUNT
    };

    RZDiskTypeTag ToDiskTag(SerializeableDataType t)
    {
        switch (t) {
            default:
            case SerializeableDataType::kPrimitive: return RZDiskTypeTag::kPrimitive;
            case SerializeableDataType::kBlob: return RZDiskTypeTag::kBlob;
            case SerializeableDataType::kArray: return RZDiskTypeTag::kArray;
            case SerializeableDataType::kHashMap: return RZDiskTypeTag::kHashMap;
            case SerializeableDataType::kString: return RZDiskTypeTag::kString;
            case SerializeableDataType::kObject: return RZDiskTypeTag::kObject;
            case SerializeableDataType::kObjectArray: return RZDiskTypeTag::kObjectArray;
            case SerializeableDataType::kEnum: return RZDiskTypeTag::kEnum;
            case SerializeableDataType::kBitField: return RZDiskTypeTag::kBitField;
        }
        return RZDiskTypeTag::kPrimitive;
    }

    //-------------------------------------------------------------------------
    //  Serialized Data types
    //-------------------------------------------------------------------------

    struct RZSerializedPrimitive
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

    struct RZSerializedBlob
    {
        u32                 offset;      // offset in the file where the blob data starts
        u32                 size;        // size of the blob data in bytes
        u32                 typeHash;    // Do we really need this? leave it for future use
        rz_compression_type compression;
        u8                  _pad0[3];
        u32                 decompressedSize;    // size after decompression, useful for allocating memory during deserialization
        u32                 _pad1[2];
    };
    static_assert(sizeof(RZSerializedBlob) == 32, "RZSerializedBlob size must be 32 bytes");

    // Array of anything
    struct RZSerializedArray
    {
        RZSerializedBlob data;    // blob of all elements
        u32              elementCount;
        u8               elementSize;
        u8               _pad0[3];
    };

    // HashMap
    struct RZSerializedHashMap
    {
        RZSerializedBlob keys;      // blob of all keys
        RZSerializedBlob values;    // blob of all values
        u32              capacity;
        u32              count;
        u32              index;
    };

    // String
    struct RZSerializedString
    {
        RZSerializedBlob data;    // blob of characters
        u32              length;
        u8               encoding;    // UTF-8, UTF-16, ASCII
        u8               _pad0[3];
    };

    // Struct instance - only works for POD/simple structs without pointers. Marked as clean POD in reflection registry.
    struct RZSerializedObject
    {
        RZSerializedBlob data;    // blob of struct bytes
        u32              typeHash;
        u32              size;
        bool             bIsTriviallySerializable;
    };

    // Array of objects - only works for POD/simple structs without pointers. Marked as clean POD in reflection registry.
    struct RZSerializedObjectArray
    {
        RZSerializedBlob data;    // blob containing array of blobs
        u32              elementCount;
        u32              elementTypeHash;
    };

    struct RZSerializedEnumValue
    {
        RZString name;
        int      value;
    };

    struct RZSerializedBitFieldValue
    {
        RZString name;
        int      bitIndex;
    };

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
            RZDynamicArray<u8>  buffer = {};
            const TypeMetaData* meta   = getTypeMetaData();
            if (!meta) {
                RAZIX_CORE_ERROR("[RZSerializable] Type metadata for type '{}' not found.", typeid(Derived).name());
                return buffer;
            }

            const u8* base = reinterpret_cast<const u8*>(&data);

            // we don't need to serialize member by member if the whole struct is trivially serializable
            // all are assumed to be primitive types
            if (meta->bIsTriviallySerializable) {
                RAZIX_CORE_TRACE("Serializing trivially serializable type: {} of size: {}", meta->name.c_str(), meta->size);
                buffer.resize(meta->size);
                memcpy(buffer.data(), base, meta->size);
                return buffer;
            }

            // Otherwise serialize member by member
            for (const auto& member: meta->members) {
                size_t oldSize = buffer.size();

                buffer.resize(oldSize + member.size);
                RAZIX_CORE_TRACE("member.name: {}, member.typeName: {}, member.offset: {}, member.size: {}", member.name.c_str(), member.typeName.c_str(), member.offset, member.size);

                switch (member.dataType) {
                    case SerializeableDataType::kPrimitive:
                        RAZIX_CORE_TRACE("Serializing primitive member: {} of size: {}", member.name.c_str(), member.size);
                        memcpy(buffer.data() + oldSize, base + member.offset, member.size);
                        break;
                    case SerializeableDataType::kBlob: {
                        RAZIX_CORE_TRACE("Serializing blob member: {} of size: {}", member.name.c_str(), member.size);
                        RZSerializedBlob blob = {};
                        blob.size             = member.size;
                        // NOTE: Currently we support only inline payloads, file offfsets will handled in V2 of the serialization system
                        // NOTE: Which means offset will be size of RZSerializedBlob and data will be written right after it and is relative to Header
                        blob.offset           = sizeof(RZSerializedBlob);    // TODO: will be filled during file writing
                        blob.typeHash         = 0;                           // future use
                        blob.compression      = RZ_COMPRESSION_NONE;
                        blob.decompressedSize = member.size;
                        memcpy(buffer.data() + oldSize, &blob, sizeof(RZSerializedBlob));

                        // Write the inline payload right after the blob metadata
                        const void* blobSrc = *reinterpret_cast<void* const*>(base + member.offset);
                        RAZIX_CORE_ASSERT(blobSrc != NULL, "Blob payload is null");
                        memcpy(buffer.data() + oldSize + blob.offset, blobSrc, blob.size);

                        break;
                    }
                    default:
                        RAZIX_DEBUG_BREAK();
                        RAZIX_CORE_ERROR("Work in progress handling other serialization types");
                        RAZIX_CORE_ERROR("Serializing non-trivially serializable member: {} of type: {}. Ensure custom serialization is handled!", member.name.c_str(), member.typeName.c_str());
                        break;
                }
            }

            return buffer;
        }

        static Derived deserializeFromBinary(const RZDynamicArray<u8>& binary)
        {
            Derived data = {};

            const TypeMetaData* meta = getTypeMetaData();
            if (!meta) {
                RAZIX_CORE_ERROR("[RZSerializable] Type metadata for type '{}' not found.", typeid(Derived).name());
                return data;
            }

            u8* dest = reinterpret_cast<u8*>(&data);

            if (meta->bIsTriviallySerializable) {
                RAZIX_CORE_TRACE("De-Serializing trivially serializable type: {} of size: {}", meta->name.c_str(), meta->size);
                memcpy(dest, binary.data(), rz_min<size_t>(meta->size, binary.size()));
                return data;
            }

            RAZIX_DEBUG_BREAK();
            RAZIX_CORE_ERROR("Work in progress handling other deserialization types");

            return data;
        }
    };
}    // namespace Razix
#endif    // _RZ_SERIALIZABLE_H

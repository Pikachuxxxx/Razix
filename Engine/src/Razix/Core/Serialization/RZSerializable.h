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

        static RZDynamicArray<u8> serializeToBinary(const Derived& instance)
        {
            RZDynamicArray<u8>  buffer = {};
            const TypeMetaData* meta   = getTypeMetaData();
            if (!meta) {
                RAZIX_CORE_ERROR("[RZSerializable] Type metadata for type '{}' not found.", typeid(Derived).name());
                return buffer;
            }

            const u8* base = reinterpret_cast<const u8*>(&instance);

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

                RAZIX_CORE_TRACE("member.name: {}, member.typeName: {}, member.offset: {}, member.size: {}", member.name.c_str(), member.typeName.c_str(), member.offset, member.trivial.size);

                // Assets to Disk Tag, makes format immune to internal SerializeableDataType changes
                RZDiskTypeTag tag = ToDiskTag(member.dataType);

                switch (tag) {
                    case RZDiskTypeTag::kPrimitive: {
                        RAZIX_CORE_TRACE("Serializing primitive member: {} of size: {}", member.name.c_str(), member.trivial.size);
                        buffer.resize(oldSize + member.trivial.size);
                        memcpy(buffer.data() + oldSize, base + member.offset, member.trivial.size);

                        break;
                    }
                    case RZDiskTypeTag::kBlob: {
                        RAZIX_CORE_TRACE("Serializing blob member: {} of size: {}", member.name.c_str(), member.trivial.size);
                        size_t writeSize = oldSize + member.trivial.size + sizeof(RZSerializedBlob);
                        buffer.resize(writeSize);

                        RZSerializedBlob blob = {};
                        blob.size             = member.trivial.size;
                        // NOTE: Currently we support only inline payloads, file offsets will handled in V2 of the serialization system
                        // NOTE: Which means offset will be size of RZSerializedBlob and data will be written right after it and is relative to Header
                        blob.offset           = sizeof(RZSerializedBlob);    // TODO: will be filled during file writing
                        blob.typeHash         = 0;                           // future use
                        blob.compression      = RZ_COMPRESSION_NONE;
                        blob.decompressedSize = member.trivial.size;
                        memcpy(buffer.data() + oldSize, &blob, sizeof(RZSerializedBlob));

                        // Write the inline payload right after the blob metadata
                        const void* blobSrc = *reinterpret_cast<void* const*>(base + member.offset);
                        RAZIX_CORE_ASSERT(blobSrc != NULL, "Blob payload is null");
                        memcpy(buffer.data() + oldSize + blob.offset, blobSrc, blob.size);

                        break;
                    }
                    case RZDiskTypeTag::kArray: {
                        RAZIX_CORE_TRACE("Serializing array member: {} of element size: {}", member.name.c_str(), member.array.elementSize);

                        RAZIX_CORE_ASSERT(member.array.ops.get_data != NULL, "Array get_data function pointer is null");
                        RAZIX_CORE_ASSERT(member.isStaticCompileSizedFixed || member.array.ops.get_size != NULL, "Array get_size function pointer is null");

                        const void* arr  = reinterpret_cast<const void*>(base + member.offset);
                        const void* data = member.array.ops.get_data(arr);
                        RAZIX_CORE_ASSERT(data != NULL, "Array data pointer is null");

                        size_t cnt   = member.isStaticCompileSizedFixed ? member.array.elementCount : member.array.ops.get_size(arr);
                        size_t bytes = cnt * member.array.elementSize;
                        RAZIX_CORE_WARN("Array member: {} has {} elements of size: {}, total bytes: {}", member.name.c_str(), cnt, member.array.elementSize, bytes);
                        size_t writeSize = oldSize + sizeof(RZSerializedArray) + bytes;
                        buffer.resize(writeSize);

                        // Create blob for array data
                        RZSerializedBlob blob = {};
                        blob.size             = static_cast<u32>(bytes);
                        blob.offset           = sizeof(RZSerializedArray);    // TODO: will be filled during file writing
                        blob.typeHash         = 0;                            // future use
                        blob.compression      = RZ_COMPRESSION_NONE;
                        blob.decompressedSize = static_cast<u32>(bytes);

                        RZSerializedArray serializedArray = {};
                        serializedArray.data              = blob;
                        serializedArray.elementSize       = member.array.elementSize;
                        serializedArray.elementCount      = static_cast<u32>(cnt);
                        memcpy(buffer.data() + oldSize, &serializedArray, sizeof(RZSerializedArray));

                        // now write the inline blob payload
                        memcpy(buffer.data() + oldSize + blob.offset, data, blob.size);

                        break;
                    }
                    default:
                        RAZIX_CORE_ERROR("Work in progress handling other serialization types");
                        RAZIX_CORE_ERROR("Serializing non-trivially serializable member: {} of type: {}. Ensure custom serialization is handled!", member.name.c_str(), member.typeName.c_str());
                        RAZIX_DEBUG_BREAK();
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

            size_t readOffset = 0;
            // Otherwise serialize member by member
            for (const auto& member: meta->members) {
                RAZIX_CORE_TRACE("member.name: {}, member.typeName: {}, member.offset: {}, member.size: {}", member.name.c_str(), member.typeName.c_str(), member.offset, member.trivial.size);

                // Assets to Disk Tag, makes format immune to internal SerializeableDataType changes
                RZDiskTypeTag tag = ToDiskTag(member.dataType);

                switch (tag) {
                    case RZDiskTypeTag::kPrimitive: {
                        RAZIX_CORE_TRACE("De-Serializing primitive member: {} of size: {}", member.name.c_str(), member.trivial.size);
                        memcpy(dest + member.offset, binary.data() + readOffset, member.trivial.size);
                        readOffset += member.trivial.size;

                        break;
                    }
                    case RZDiskTypeTag::kBlob: {
                        RAZIX_CORE_TRACE("De-Serializing blob member: {} of size: {}", member.name.c_str(), member.trivial.size);
                        RZSerializedBlob blob = {};
                        memcpy(&blob, binary.data() + readOffset, sizeof(RZSerializedBlob));
                        readOffset += sizeof(RZSerializedBlob);    // move to the payload start

                        // TESTING ONLY! memory allocation for blob payload
                        // FIXME: don't allocate memory, pass a pre-allocated buffer from outside,
                        // FIXME: the Derived should be passed with pre-allocated memory for blobs
                        // TODO: Or even better pass a scratch allocator to allocate memory from
                        void* payload = rz_malloc_aligned(blob.decompressedSize);
                        memcpy(payload, binary.data() + readOffset, blob.size);
                        readOffset += blob.size;

                        // store the pointer into the struct
                        *reinterpret_cast<void**>(dest + member.offset) = payload;

                        break;
                    }
                    case RZDiskTypeTag::kArray: {
                        RAZIX_CORE_TRACE("De-Serializing array member: {} of element size: {}", member.name.c_str(), member.array.elementSize);

                        RZSerializedArray serializedArray = {};
                        memcpy(&serializedArray, binary.data() + readOffset, sizeof(RZSerializedArray));
                        readOffset += sizeof(RZSerializedArray);

                        u32 loadedDataSize = serializedArray.data.size;
                        RAZIX_CORE_TRACE("Array member: {} has {} elements of size: {}, total bytes: {} and member.array.elementCount: {}", member.name.c_str(), serializedArray.elementCount, serializedArray.elementSize, loadedDataSize, member.array.elementCount);
                        RAZIX_CORE_ASSERT(!member.isStaticCompileSizedFixed || serializedArray.elementCount == member.array.elementCount, "Static compile sized fixed array size mismatch during deserialization");
                        // memory allocation for array is done internally as-usualy, unless containers use custom allocators
                        member.array.ops.set_size(reinterpret_cast<void*>(dest + member.offset), serializedArray.elementCount);
                        member.array.ops.set_data(reinterpret_cast<void*>(dest + member.offset), reinterpret_cast<const void*>(binary.data() + readOffset));
                        readOffset += loadedDataSize;

                        break;
                    }
                    default:
                        RAZIX_CORE_ERROR("Work in progress handling other deserialization types");
                        RAZIX_CORE_ERROR("De-Serializing non-trivially serializable member: {} of type: {}. Ensure custom deserialization is handled!", member.name.c_str(), member.typeName.c_str());
                        RAZIX_DEBUG_BREAK();
                        break;
                }
            }
            return data;
        }
    };
}    // namespace Razix
#endif    // _RZ_SERIALIZABLE_H

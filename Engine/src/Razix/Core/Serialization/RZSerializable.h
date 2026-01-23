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
        u32              count;
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

    private:
        enum class RZArchiveMode
        {
            kRead,
            kWrite,
            COUNT
        };

        struct RZBinaryArchive
        {
            RZDynamicArray<u8>* buffer;
            size_t              cursor = 0;
            RZArchiveMode       mode;

            void write(const void* src, size_t size)
            {
                size_t oldSize = buffer->size();
                buffer->resize(oldSize + size);
                memcpy(buffer->data() + oldSize, src, size);
            }

            void read(void* dst, size_t size)
            {
                memcpy(dst, buffer->data() + cursor, size);
                cursor += size;
            }
        };

        static void processPrimitive(RZBinaryArchive& ar, u8* base, const MemberMetaData& member)
        {
            if (ar.mode == RZArchiveMode::kWrite) {
                // all are assumed to be primitive types
                ar.write(base + member.offset, member.trivial.size);
            } else {
                ar.read(base + member.offset, member.trivial.size);
            }
        }

        static void processBlob(RZBinaryArchive& ar, u8* base, const MemberMetaData& member)
        {
            if (ar.mode == RZArchiveMode::kWrite) {
                // Assets to Disk Tag, makes format immune to internal SerializeableDataType changes
                RZSerializedBlob blob = {};
                blob.size             = member.trivial.size;
                // NOTE: Currently we support only inline payloads, file offsets will handled in V2 of the serialization system
                // NOTE: Which means offset will be size of RZSerializedBlob and data will be written right after it and is relative to Header
                blob.offset           = sizeof(RZSerializedBlob);    // TODO: will be filled during file writing
                blob.typeHash         = 0;                           // future use
                blob.compression      = RZ_COMPRESSION_NONE;
                blob.decompressedSize = member.trivial.size;

                ar.write(&blob, sizeof(RZSerializedBlob));

                // Write the inline payload right after the blob metadata
                const void* blobSrc = *reinterpret_cast<void* const*>(base + member.offset);
                RAZIX_CORE_ASSERT(blobSrc != NULL, "Blob payload is null");
                ar.write(blobSrc, blob.size);
            } else {
                RZSerializedBlob blob = {};
                ar.read(&blob, sizeof(RZSerializedBlob));

                // TESTING ONLY! memory allocation for blob payload
                // FIXME: don't allocate memory, pass a pre-allocated buffer from outside,
                // FIXME: the Derived should be passed with pre-allocated memory for blobs
                // TODO: Or even better pass a scratch allocator to allocate memory from
                void* payload = rz_malloc_aligned(blob.decompressedSize);
                ar.read(payload, blob.size);

                // store the pointer into the struct
                *reinterpret_cast<void**>(base + member.offset) = payload;
            }
        }

        static void processArray(RZBinaryArchive& ar, u8* base, const MemberMetaData& member)
        {
            RAZIX_CORE_ASSERT(member.array.ops.get_data != NULL, "Array get_data function pointer is null");
            RAZIX_CORE_ASSERT(member.isStaticCompileSizedFixed || member.array.ops.get_size != NULL,
                "Array get_size function pointer is null");
            RAZIX_CORE_ASSERT(member.array.ops.set_data != NULL, "Array set_data function pointer is null");
            RAZIX_CORE_ASSERT(member.array.ops.set_size != NULL, "Array set_size function pointer is null");

            if (ar.mode == RZArchiveMode::kWrite) {
                const void* arr  = reinterpret_cast<const void*>(base + member.offset);
                const void* data = member.array.ops.get_data(arr);
                RAZIX_CORE_ASSERT(data != NULL, "Array data pointer is null");

                size_t cnt = member.isStaticCompileSizedFixed
                                 ? member.array.elementCount
                                 : member.array.ops.get_size(arr);

                size_t bytes = cnt * member.array.elementSize;

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

                ar.write(&serializedArray, sizeof(RZSerializedArray));

                // now write the inline blob payload
                ar.write(data, blob.size);
            } else {
                RZSerializedArray serializedArray = {};
                ar.read(&serializedArray, sizeof(RZSerializedArray));

                RAZIX_CORE_ASSERT(
                    !member.isStaticCompileSizedFixed ||
                        serializedArray.elementCount == member.array.elementCount,
                    "Static compile sized fixed array size mismatch during deserialization");

                // memory allocation for array is done internally as-usualy, unless containers use custom allocators
                void* arr = reinterpret_cast<void*>(base + member.offset);
                member.array.ops.set_size(arr, serializedArray.elementCount);
                member.array.ops.set_data(arr, ar.buffer->data() + ar.cursor);

                ar.cursor += serializedArray.data.size;
            }
        }

        static void proceessString(RZBinaryArchive& ar, u8* base, const MemberMetaData& member)
        {
            RAZIX_CORE_ASSERT(member.string.ops.get_data != NULL, "String get_data function pointer is null");
            RAZIX_CORE_ASSERT(member.string.ops.get_size != NULL, "String get_length function pointer is null");
            RAZIX_CORE_ASSERT(member.string.ops.set_data != NULL, "String set_data function pointer is null");
            RAZIX_CORE_ASSERT(member.string.ops.set_size != NULL, "String set_size function pointer is null");

            if (ar.mode == RZArchiveMode::kWrite) {
                const void* strData = member.string.ops.get_data(reinterpret_cast<const void*>(base + member.offset));
                size_t      length  = member.string.ops.get_size(reinterpret_cast<const void*>(base + member.offset)) + 1;    // +1 for null terminator

                RZSerializedBlob blob = {};
                blob.size             = static_cast<u32>(length);
                blob.offset           = sizeof(RZSerializedString);    // TODO: will be filled during file writing
                blob.typeHash         = 0;                             // future use
                blob.compression      = RZ_COMPRESSION_NONE;
                blob.decompressedSize = static_cast<u32>(length);

                RZSerializedString serializedString = {};
                serializedString.data               = blob;
                serializedString.length             = static_cast<u32>(length);
                serializedString.encoding           = 0;    // TODO: set encoding type

                ar.write(&serializedString, sizeof(RZSerializedString));

                // now write the inline blob payload
                ar.write(strData, blob.size);
            } else {
                RZSerializedString serializedString = {};
                ar.read(&serializedString, sizeof(RZSerializedString));

                // memory allocation for string is done internally as-usualy, unless containers use custom allocators
                void* str = reinterpret_cast<void*>(base + member.offset);
                member.string.ops.set_size(str, serializedString.length);
                member.string.ops.set_data(str, ar.buffer->data() + ar.cursor);

                ar.cursor += serializedString.data.size;
            }
        }

        static void processHashMap(RZBinaryArchive& ar, u8* base, const MemberMetaData& member)
        {
            RAZIX_CORE_ASSERT(member.map.ops.get_keys != NULL, "HashMap get_keys function pointer is null");
            RAZIX_CORE_ASSERT(member.map.ops.get_values != NULL, "HashMap get_values function pointer is null");
            RAZIX_CORE_ASSERT(member.isStaticCompileSizedFixed || member.map.ops.get_size != NULL,
                "HashMap get_size function pointer is null");
            RAZIX_CORE_ASSERT(member.map.ops.set_data != NULL, "HashMap ops set_data function pointer is null");
            RAZIX_CORE_ASSERT(member.map.ops.set_size != NULL, "HashMap ops set_size function pointer is null");

            if (ar.mode == RZArchiveMode::kWrite) {
                size_t      count    = member.string.ops.get_size(reinterpret_cast<const void*>(base + member.offset));
                const void* keysData = member.map.ops.get_keys(reinterpret_cast<const void*>(base + member.offset));
                RAZIX_CORE_ASSERT(count == 0 || keysData != NULL, "HashMap keys data pointer is null");
                const void* valuesData = member.map.ops.get_values(reinterpret_cast<const void*>(base + member.offset));
                RAZIX_CORE_ASSERT(count == 0 || valuesData != NULL, "HashMap values data pointer is null");

                u32 keyBytes   = count * member.map.keySize;
                u32 valueBytes = count * member.map.valueSize;

                RAZIX_CORE_TRACE("Serializing HashMap with {} entries ({} bytes keys, {} bytes values)", count, keyBytes, valueBytes);

                RZSerializedBlob keysBlob = {};
                keysBlob.size             = keyBytes;
                keysBlob.offset           = sizeof(RZSerializedHashMap);    // TODO: will be filled during file writing
                keysBlob.typeHash         = 0;                              // future use
                keysBlob.compression      = RZ_COMPRESSION_NONE;
                keysBlob.decompressedSize = keyBytes;

                RZSerializedBlob valuesBlob = {};
                valuesBlob.size             = valueBytes;
                valuesBlob.offset           = sizeof(RZSerializedHashMap) + keysBlob.size;
                valuesBlob.typeHash         = 0;    // future use
                valuesBlob.compression      = RZ_COMPRESSION_NONE;
                valuesBlob.decompressedSize = valueBytes;

                RZSerializedHashMap serializedHashMap = {};
                serializedHashMap.count               = static_cast<u32>(count);
                serializedHashMap.keys                = keysBlob;
                serializedHashMap.values              = valuesBlob;

                ar.write(&serializedHashMap, sizeof(RZSerializedHashMap));

                // now write the inline blob payload
                ar.write(keysData, keysBlob.size);
                ar.write(valuesData, valuesBlob.size);
            } else {
                RZSerializedHashMap serializedHashMap = {};
                ar.read(&serializedHashMap, sizeof(RZSerializedHashMap));

                // memory allocation for hashmap is done internally as-usualy, unless containers use custom allocators
                void* map = reinterpret_cast<void*>(base + member.offset);

                member.map.ops.set_size(map, serializedHashMap.count);

                const void* keysData   = ar.buffer->data() + ar.cursor;
                const void* valuesData = ar.buffer->data() + ar.cursor + (serializedHashMap.count * member.map.keySize);
                member.map.ops.set_data(map, keysData, valuesData);

                // advance the cursor by total size of keys and values blobs
                ar.cursor += (member.map.keySize + member.map.valueSize) * serializedHashMap.count;
            }
        }

        // static void processObject(RZBinaryArchive& ar, u8* base, const TypeMetaData& meta)
        // {
        //     // we don't need to serialize member by member if the whole struct is trivially serializable
        //     if (meta->bIsTriviallySerializable) {
        //         if (ar.mode == RZArchiveMode::Write)
        //             ar.write(object, meta->size);
        //         else
        //             ar.read(object, meta->size);
        //         return;
        //     }
        //
        //     // Otherwise serialize/deserialize member by member
        //     for (const auto& member: meta->members)
        //         processMember(ar, object, member);
        // }

        static void processMember(RZBinaryArchive& ar, void* objectBase, const MemberMetaData& member)
        {
            u8* base = reinterpret_cast<u8*>(objectBase);

            RAZIX_CORE_TRACE("Processing member '{}' of type '{}' at offset {}",
                member.name, member.typeName, member.offset);

            // Assets to Disk Tag, makes format immune to internal SerializeableDataType changes
            switch (ToDiskTag(member.dataType)) {
                case RZDiskTypeTag::kPrimitive:
                    processPrimitive(ar, base, member);
                    break;
                case RZDiskTypeTag::kBlob:
                    processBlob(ar, base, member);
                    break;
                case RZDiskTypeTag::kArray:
                    processArray(ar, base, member);
                    break;
                case RZDiskTypeTag::kString:
                    proceessString(ar, base, member);
                    break;
                case RZDiskTypeTag::kHashMap:
                    processHashMap(ar, base, member);
                    break;
                default:
                    RAZIX_CORE_ERROR("Work in progress handling other serialization types");
                    RAZIX_DEBUG_BREAK();
                    break;
            }
        }

    public:
        static RZDynamicArray<u8> serializeToBinary(const Derived& instance)
        {
            RZDynamicArray<u8> buffer = {};

            const TypeMetaData* meta = getTypeMetaData();
            if (!meta) {
                RAZIX_CORE_ERROR("[RZSerializable] Type metadata for type '{}' not found.",
                    typeid(Derived).name());
                return buffer;
            }

            RZBinaryArchive ar{&buffer, 0, RZArchiveMode::kWrite};

            // we don't need to serialize member by member if the whole struct is trivially serializable
            if (meta->bIsTriviallySerializable) {
                buffer.resize(meta->size);
                memcpy(buffer.data(), &instance, meta->size);
                return buffer;
            }

            // Otherwise serialize member by member
            for (const auto& member: meta->members)
                processMember(ar, const_cast<Derived*>(&instance), member);

            return buffer;
        }

        static Derived deserializeFromBinary(const RZDynamicArray<u8>& binary)
        {
            Derived data = {};

            const TypeMetaData* meta = getTypeMetaData();
            if (!meta) {
                RAZIX_CORE_ERROR("[RZSerializable] Type metadata for type '{}' not found.",
                    typeid(Derived).name());
                return data;
            }

            RZDynamicArray<u8> temp = binary;
            RZBinaryArchive    ar{&temp, 0, RZArchiveMode::kRead};

            if (meta->bIsTriviallySerializable) {
                memcpy(&data, binary.data(), rz_min<size_t>(meta->size, binary.size()));
                return data;
            }

            // Otherwise deserialize member by member
            for (const auto& member: meta->members)
                processMember(ar, &data, member);

            return data;
        }
    };

}    // namespace Razix
#endif    // _RZ_SERIALIZABLE_H

#ifndef _RZ_SERIALIZABLE_H
#define _RZ_SERIALIZABLE_H

#include "Razix/Core/std/type_traits.h"
#include "Razix/Core/std/utility.h"

#include "Razix/Core/Compression/RZCompression.h"

#include "Razix/Core/Reflection/RZReflectionRegistry.h"
#include <Core/Containers/arrays.h>
#include <Core/Memory/Allocators/RZHeapAllocator.h>
#include <Core/RZCore.h>

namespace Razix {

    struct RZAssetColdData;
    class RZAsset;

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
        kUUID        = 10,
        COUNT
    };

    // Assets to Disk Tag, makes format immune to internal SerializeableDataType changes
    RAZIX_API RZDiskTypeTag SerializableTypeToDiskTag(SerializeableDataType t);

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
        u32                 size;        // size of the blob data in bytes (compressed size on disk)
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
        RZSerializedBlob keysValues;    // blob of all keys and Values interleaved
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

    // Struct instance - only works for POD/simpRZSerializer (yes rename it!) le structs without pointers. Marked as clean POD in reflection registry.
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

    RAZIX_API void* RZAssetUtilCreateAssetInstanceInPlace(void* memoryBacking, void* coldDataBacking);

    enum class RZArchiveMode
    {
        kRead,
        kWrite,
        COUNT
    };

    // TODO: Stop using RZDynamicArray<u8> maybe, redirect allocations via scratch buffer?

    // Inline payload binary archive
    struct RZBinaryArchive
    {
        RZDynamicArray<u8>*      buffer = NULL;
        size_t                   cursor = 0;
        RZArchiveMode            mode;
        Memory::RZHeapAllocator* heapAllocator       = nullptr;
        static constexpr bool    kUsesOutOfLineBlobs = false;

        RZBinaryArchive() {}

        RZBinaryArchive(RZDynamicArray<u8>* arr, size_t cursor, RZArchiveMode mode)
            : buffer(arr), cursor(cursor), heapAllocator(nullptr), mode(mode) {}

        RZBinaryArchive(RZDynamicArray<u8>* arr, size_t cursor, RZArchiveMode mode, Memory::RZHeapAllocator* allocator)
            : buffer(arr), cursor(cursor), heapAllocator(allocator), mode(mode) {}

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

        void readBlob(const RZSerializedBlob& blob, void** targetPtr)
        {
            RAZIX_CORE_ASSERT(heapAllocator != NULL || *targetPtr != NULL, "[Serializer] Heap allocator required for blob deserialization");

            void* payload = *targetPtr != NULL ? *targetPtr : heapAllocator->allocate(static_cast<size_t>(blob.decompressedSize));
            RAZIX_CORE_ASSERT(payload != nullptr, "[Serializer] Heap allocator failed to allocate blob payload");

            memcpy(payload, buffer->data() + cursor, blob.size);
            cursor += blob.size;
            *targetPtr = payload;
        }

        void readArray(const RZSerializedBlob& blob, void* instance, ArrayOps ops)
        {
            ops.set_data(instance, buffer->data() + cursor);
            cursor += blob.size;
        }

        void readString(const RZSerializedBlob& blob, void* instance, StringOps ops)
        {
            ops.set_data(instance, buffer->data() + cursor);
            cursor += blob.size;
        }

        void readHashMap(const RZSerializedBlob& blob, void* instance, HashMapOps ops)
        {
            ops.set_data(instance, buffer->data() + cursor);
            cursor += blob.size;
        }
    };

#ifdef RAZIX_USE_COMPRESSED_ARCHIVE

    struct RZPendingWriteBlob
    {
        size_t              headerOffset;      // offset inside headerBuffer
        const void*         payload;           // original uncompressed payload pointer
        u32                 payloadSize;       // uncompressed size
        u32                 compressedSize;    // will be filled later when compression is being done
        rz_compression_type compression;       // compression type
    };

    struct RZPendingReadBlob
    {
        u64                   fileOffset;    // absolute offset in file
        u32                   compressedSize;
        u32                   decompressedSize;
        rz_compression_type   compression;
        void*                 targetInstance;
        SerializeableDataType targetType;
        union
        {
            ArrayOps   arrayOps;
            StringOps  stringOps;
            HashMapOps hashMapOps;
        };
    };

    #define RAZIX_ASSSET_FILE_MAGIC 0x46415A52    // 'R','Z','A','F' (Razix Archive File) = 0x525A4146 [echo "RZAF" | xxd]

    struct RZFileHeader
    {
        u32 magic = RAZIX_ASSSET_FILE_MAGIC;    // 'R','Z','A','F' (Razix Archive File) = 0x525A4146 [echo "RZAF" | xxd]
        u32 version;
        u64 headerSectionSize;     // size of the header section
        u64 payloadSectionSize;    // size of the payload section
    };

    /**
     * Out of line compressed archive
     * Format: [RZFileHeader][HeaderSection --> one of the Serializerable meta types][COMPRESSED PayloadSection]
     */
    struct RZCompressedArchive
    {
        RZDynamicArray<u8>*   buffer = NULL;
        size_t                cursor = 0;
        RZArchiveMode         mode;
        static constexpr bool kUsesOutOfLineBlobs = true;

        // ---------- write ----------
        RZDynamicArray<u8>                 headerBuffer;
        RZDynamicArray<u8>                 payloadBuffer;
        RZDynamicArray<RZPendingWriteBlob> pendingWriteBlobs;
        // ---------- read ----------
        const u8*                         fileBase    = NULL;
        const u8*                         headerBase  = NULL;
        const u8*                         payloadBase = NULL;
        RZDynamicArray<RZPendingReadBlob> pendingReadBlobs;
        Memory::RZHeapAllocator*          heapAllocator = nullptr;

        RZCompressedArchive() {}

        RZCompressedArchive(RZDynamicArray<u8>* arr, size_t cursor, RZArchiveMode mode)
            : buffer(arr), cursor(cursor), heapAllocator(nullptr), mode(mode) {}

        RZCompressedArchive(RZDynamicArray<u8>* arr, size_t cursor, RZArchiveMode mode, Memory::RZHeapAllocator* allocator)
            : buffer(arr), cursor(cursor), heapAllocator(allocator), mode(mode) {}

        void write(const void* src, size_t size)
        {
            RAZIX_CORE_ASSERT(mode == RZArchiveMode::kWrite, "write() in read mode");

            size_t oldSize = headerBuffer.size();
            headerBuffer.resize(oldSize + size);
            memcpy(headerBuffer.data() + oldSize, src, size);
            cursor += size;
        }

        void read(void* dst, size_t size)
        {
            RAZIX_CORE_ASSERT(mode == RZArchiveMode::kRead, "read() in write mode");

            memcpy(dst, buffer->data() + cursor, size);
            cursor += size;
        }

        void registerWriteBlob(size_t headerOffset, const void* payload, u32 payloadSize, rz_compression_type compression)
        {
            RAZIX_CORE_ASSERT(payload != NULL, "[Serializer] payload pointer cannot be null");
            RAZIX_CORE_ASSERT(payloadSize > 0, "[Serializer] payload size is 0 for registered blob");

            pendingWriteBlobs.push_back({headerOffset,
                payload,
                payloadSize,
                0,    // data is not compressed yet
                compression});
        }

        void registerReadBlob(const RZSerializedBlob& blob, void** targetptr)
        {
            RAZIX_CORE_ASSERT(mode == RZArchiveMode::kRead, "registerReadBlob in write mode");
            RAZIX_CORE_ASSERT(targetptr != NULL, "outPtr is null");

            RZPendingReadBlob pending = {};
            pending.fileOffset        = static_cast<u64>(blob.offset);
            pending.compressedSize    = blob.size;
            pending.decompressedSize  = blob.decompressedSize;
            pending.compression       = blob.compression;
            pending.targetType        = SerializeableDataType::kBlob;
            pending.targetInstance    = targetptr;

            pendingReadBlobs.push_back(pending);
        }

        void registerReadType(const RZSerializedBlob& blob, void* instance, SerializeableDataType type, ArrayOps ops)
        {
            RZPendingReadBlob pending = {};
            pending.fileOffset        = static_cast<u64>(blob.offset);
            pending.compressedSize    = blob.size;
            pending.decompressedSize  = blob.decompressedSize;
            pending.compression       = blob.compression;
            pending.targetInstance    = instance;
            pending.targetType        = type;
            pending.arrayOps          = ops;

            pendingReadBlobs.push_back(pending);
        }

        void registerReadType(const RZSerializedBlob& blob, void* instance, SerializeableDataType type, HashMapOps ops)
        {
            RZPendingReadBlob pending = {};
            pending.fileOffset        = static_cast<u64>(blob.offset);
            pending.compressedSize    = blob.size;
            pending.decompressedSize  = blob.decompressedSize;
            pending.compression       = blob.compression;
            pending.targetInstance    = instance;
            pending.targetType        = type;
            pending.hashMapOps        = ops;

            pendingReadBlobs.push_back(pending);
        }

        void compressPayloads(rz_compression_type compressionType)
        {
            RAZIX_CORE_ASSERT(mode == RZArchiveMode::kWrite, "compressPayloads in read mode");

            payloadBuffer.clear();

            for (auto& pb: pendingWriteBlobs) {
                size_t payloadStart = payloadBuffer.size();

                pb.compression = compressionType;

                if (pb.compression == RZ_COMPRESSION_NONE) {
                    payloadBuffer.resize(payloadStart + pb.payloadSize);
                    memcpy(payloadBuffer.data() + payloadStart, pb.payload, pb.payloadSize);
                    pb.compressedSize = pb.payloadSize;
                } else {
                    // Allocate worst-case size to compress into
                    payloadBuffer.resize(payloadStart + pb.payloadSize);

                    size_t outCompressedFinalSize = 0;
                    rz_compress(pb.compression, pb.payload, pb.payloadSize, payloadBuffer.data() + payloadStart, &outCompressedFinalSize);

                    RAZIX_CORE_ASSERT(outCompressedFinalSize != 0 || outCompressedFinalSize <= pb.payloadSize,
                        "[Serialization] Compressed size is fishy! check blob serialization.");

                    pb.compressedSize = static_cast<u32>(outCompressedFinalSize);
                    payloadBuffer.resize(payloadStart + pb.compressedSize);
                }
            }
        }

        void decompressPayloads()
        {
            RAZIX_CORE_ASSERT(mode == RZArchiveMode::kRead, "decompressPayloads in write mode");

            RAZIX_CORE_ASSERT(heapAllocator != nullptr, "[Serializer] Heap allocator required for payload decompression");

            for (auto& rb: pendingReadBlobs) {
                const u8* src = fileBase + rb.fileOffset;

                // TODO: use a scratch buffer for serializer allocations
                void* data = heapAllocator->allocate(static_cast<size_t>(rb.decompressedSize));
                RAZIX_CORE_ASSERT(data != nullptr, "[Serializer] Heap allocator failed to allocate decompressed payload");

                if (rb.compression == RZ_COMPRESSION_NONE) {
                    memcpy(data, src, rb.decompressedSize);
                } else {
                    size_t outDecompressedSize = 0;
                    rz_decompress(rb.compression, src, rb.compressedSize, data, rb.decompressedSize, &outDecompressedSize);
                    RAZIX_CORE_ASSERT(outDecompressedSize >= rb.decompressedSize,
                        "[Serialization] decompressed blob size is less that expected, corrupted decompression has occurred!");
                }

                switch (rb.targetType) {
                    case SerializeableDataType::kBlob:
                        *reinterpret_cast<void**>(rb.targetInstance) = data;
                        break;
                    case SerializeableDataType::kArray:
                        rb.arrayOps.set_data(rb.targetInstance, data);
                        heapAllocator->deallocate(data);
                        break;
                    case SerializeableDataType::kString:
                        rb.stringOps.set_data(rb.targetInstance, data);
                        heapAllocator->deallocate(data);
                        break;
                    case SerializeableDataType::kHashMap:
                        rb.hashMapOps.set_data(rb.targetInstance, data);
                        heapAllocator->deallocate(data);
                        break;
                    default:
                        RAZIX_CORE_ERROR("Unhandled pending read blob type");
                        RAZIX_DEBUG_BREAK();
                        break;
                }
            }
        }

        void finalizeWrites()
        {
            RAZIX_CORE_ASSERT(mode == RZArchiveMode::kWrite, "finalize in read mode");

            u32 payloadCursor = 0;

            for (auto& pb: pendingWriteBlobs) {
                RZSerializedBlob* hdr =
                    reinterpret_cast<RZSerializedBlob*>(
                        headerBuffer.data() + pb.headerOffset);

                // Patch payload offsets and sizes of out-of-line blobs
                hdr->offset =
                    sizeof(RZFileHeader) +
                    static_cast<u32>(headerBuffer.size()) +
                    payloadCursor;

                hdr->size = pb.compressedSize;
                payloadCursor += pb.compressedSize;

                hdr->compression = pb.compression;
            }

            // emit final bufferWrites
            RZFileHeader fh       = {};
            fh.magic              = RAZIX_ASSSET_FILE_MAGIC;
            fh.version            = 1;
            fh.headerSectionSize  = static_cast<u32>(headerBuffer.size());
            fh.payloadSectionSize = static_cast<u32>(payloadBuffer.size());

            buffer->resize(sizeof(RZFileHeader) +
                           headerBuffer.size() +
                           payloadBuffer.size());

            u8* dst = buffer->data();

            RAZIX_CORE_INFO("Finalized archive with header size {} bytes and payload size {} bytes",
                headerBuffer.size(),
                payloadBuffer.size());

            memcpy(dst, &fh, sizeof(fh));
            if (headerBuffer.size() > 0) {
                memcpy(dst + sizeof(fh),
                    headerBuffer.data(),
                    headerBuffer.size());
            }
            if (payloadBuffer.size() > 0) {
                memcpy(dst + sizeof(fh) + headerBuffer.size(),
                    payloadBuffer.data(),
                    payloadBuffer.size());
            }
        }
    };
#endif    // RAZIX_USE_COMPRESSED_ARCHIVE

    template<typename Derived, typename Archive = RZBinaryArchive>
    class RZSerializable
    {
    public:
        // Free out-of-line blob allocations that were provided by an external heap allocator
        static void freeDeserializedBlobs(void* objectBase, Memory::RZHeapAllocator& allocator)
        {
            const TypeMetaData* meta = getTypeMetaData();
            if (!meta) {
                RAZIX_CORE_ERROR("[RZSerializable] Type metadata for type '{}' not found.", typeid(Derived).name());
                return;
            }

            auto freeMember = [&](void* basePtr, const MemberMetaData& member, auto&& freeMemberRef) -> void {
                u8* base = reinterpret_cast<u8*>(basePtr);

                switch (SerializableTypeToDiskTag(member.dataType)) {
                    case RZDiskTypeTag::kBlob: {
                        void** ptr = reinterpret_cast<void**>(base + member.offset);
                        if (*ptr != nullptr) {
                            allocator.deallocate(*ptr);
                            *ptr = nullptr;
                        }
                    } break;
                    case RZDiskTypeTag::kObject: {
                        const TypeMetaData* m = Razix::RZTypeRegistry::getTypeMetaData(member.object.type);
                        if (m && !m->bIsTriviallySerializable) {
                            void* objBase = base + member.offset;
                            for (const auto& mem: m->members)
                                freeMemberRef(objBase, mem, freeMemberRef);
                        }
                    } break;
                    default:
                        break;
                }
            };

            for (const auto& member: meta->members)
                freeMember(objectBase, member, freeMember);
        }
#ifdef RAZIX_USE_COMPRESSED_ARCHIVE
        struct RZAsyncSerializationContext
        {
            RZCompressedArchive archive;
            RZArchiveMode       mode;
            struct WriteState
            {
                RZDynamicArray<u8> resultBuffer;
            };

            struct ReadState
            {
                Derived*           targetInstance = nullptr;
                RZDynamicArray<u8> sourceBuffer;
            };

            WriteState write;
            ReadState  read;
        };
#endif    // RAZIX_USE_COMPRESSED_ARCHIVE

        virtual ~RZSerializable() = default;

        static const TypeMetaData* getTypeMetaData()
        {
            return Razix::RZTypeRegistry::getTypeMetaData<rz_remove_cv_t<rz_remove_pointer_t<Derived>>>();
        }

    private:
        static void processPrimitive(Archive& ar, u8* base, const MemberMetaData& member)
        {
            if (ar.mode == RZArchiveMode::kWrite) {
                ar.write(base + member.offset, member.trivial.size);
            } else {
                ar.read(base + member.offset, member.trivial.size);
            }
        }

        static void processBlob(Archive& ar, u8* base, const MemberMetaData& member)
        {
            if (ar.mode == RZArchiveMode::kWrite) {
                RZSerializedBlob blob = {};
                blob.size             = member.trivial.size;
                blob.offset           = 0;    // patched for compressed archive
                blob.typeHash         = 0;
                blob.compression      = RZ_COMPRESSION_NONE;
                blob.decompressedSize = member.trivial.size;

                size_t headerOffset = ar.cursor;
                ar.write(&blob, sizeof(RZSerializedBlob));

                const void* blobSrc = *reinterpret_cast<void* const*>(base + member.offset);
                RAZIX_CORE_ASSERT(blobSrc != NULL, "Blob payload is null");

                if constexpr (Archive::kUsesOutOfLineBlobs) {
                    ar.registerWriteBlob(headerOffset, blobSrc, blob.decompressedSize, blob.compression);
                } else {
                    ar.write(blobSrc, blob.size);
                }

            } else {
                RZSerializedBlob blob = {};
                ar.read(&blob, sizeof(RZSerializedBlob));

                void** targetPtr = reinterpret_cast<void**>(base + member.offset);
                if constexpr (Archive::kUsesOutOfLineBlobs) {
                    ar.registerReadBlob(blob, targetPtr);
                } else {
                    ar.readBlob(blob, targetPtr);
                }
            }
        }

        static void processArray(Archive& ar, u8* base, const MemberMetaData& member)
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

                RZSerializedBlob blob = {};
                blob.size             = static_cast<u32>(bytes);
                blob.offset           = 0;
                blob.typeHash         = 0;
                blob.compression      = RZ_COMPRESSION_NONE;
                blob.decompressedSize = static_cast<u32>(bytes);

                RZSerializedArray serializedArray = {};
                serializedArray.data              = blob;
                serializedArray.elementSize       = member.array.elementSize;
                serializedArray.elementCount      = static_cast<u32>(cnt);

                size_t headerOffset = ar.cursor;
                ar.write(&serializedArray, sizeof(RZSerializedArray));

                if constexpr (Archive::kUsesOutOfLineBlobs) {
                    ar.registerWriteBlob(headerOffset, data, blob.decompressedSize, blob.compression);
                } else {
                    ar.write(data, blob.size);
                }
            } else {
                RZSerializedArray serializedArray = {};
                ar.read(&serializedArray, sizeof(RZSerializedArray));

                RAZIX_CORE_ASSERT(
                    !member.isStaticCompileSizedFixed ||
                        serializedArray.elementCount == member.array.elementCount,
                    "Static compile sized fixed array size mismatch during deserialization");

                void* arr = reinterpret_cast<void*>(base + member.offset);
                member.array.ops.set_size(arr, serializedArray.elementCount);

                if constexpr (Archive::kUsesOutOfLineBlobs) {
                    ar.registerReadType(serializedArray.data, arr, SerializeableDataType::kArray, member.array.ops);
                } else {
                    ar.readArray(serializedArray.data, arr, member.array.ops);
                }
            }
        }

        static void proceessString(Archive& ar, u8* base, const MemberMetaData& member)
        {
            RAZIX_CORE_ASSERT(member.string.ops.get_data != NULL, "String get_data function pointer is null");
            RAZIX_CORE_ASSERT(member.string.ops.get_size != NULL, "String get_length function pointer is null");
            RAZIX_CORE_ASSERT(member.string.ops.set_data != NULL, "String set_data function pointer is null");
            RAZIX_CORE_ASSERT(member.string.ops.set_size != NULL, "String set_size function pointer is null");

            if (ar.mode == RZArchiveMode::kWrite) {
                const void* strData = member.string.ops.get_data(reinterpret_cast<const void*>(base + member.offset));
                size_t      length  = member.string.ops.get_size(reinterpret_cast<const void*>(base + member.offset)) + 1;

                RZSerializedBlob blob = {};
                blob.size             = static_cast<u32>(length);
                blob.offset           = 0;
                blob.typeHash         = 0;
                blob.compression      = RZ_COMPRESSION_NONE;
                blob.decompressedSize = static_cast<u32>(length);

                RZSerializedString serializedString = {};
                serializedString.data               = blob;
                serializedString.length             = static_cast<u32>(length);
                serializedString.encoding           = 0;

                size_t headerOffset = ar.cursor;
                ar.write(&serializedString, sizeof(RZSerializedString));

                if constexpr (Archive::kUsesOutOfLineBlobs) {
                    ar.registerWriteBlob(headerOffset, strData, blob.decompressedSize, blob.compression);
                } else {
                    ar.write(strData, blob.size);
                }
            } else {
                RZSerializedString serializedString = {};
                ar.read(&serializedString, sizeof(RZSerializedString));

                void* str = reinterpret_cast<void*>(base + member.offset);
                member.string.ops.set_size(str, serializedString.length);

                if constexpr (Archive::kUsesOutOfLineBlobs) {
                    ar.registerReadType(serializedString.data, str, SerializeableDataType::kString, member.string.ops);
                } else {
                    ar.readString(serializedString.data, str, member.string.ops);
                }
            }
        }

        static void processHashMap(Archive& ar, u8* base, const MemberMetaData& member)
        {
            RAZIX_CORE_ASSERT(member.map.ops.get_data != NULL, "HashMap get_data function pointer is null");
            RAZIX_CORE_ASSERT(member.map.ops.get_size != NULL, "HashMap get_size function pointer is null");
            RAZIX_CORE_ASSERT(member.map.ops.set_data != NULL, "HashMap ops set_data function pointer is null");
            RAZIX_CORE_ASSERT(member.map.ops.set_size != NULL, "HashMap ops set_size function pointer is null");

            RAZIX_CORE_ASSERT(
                (member.map.keyType == typeid(u8) || member.map.keyType == typeid(i8) ||
                    member.map.keyType == typeid(u16) || member.map.keyType == typeid(i16) ||
                    member.map.keyType == typeid(u32) || member.map.keyType == typeid(i32) ||
                    member.map.keyType == typeid(u64) || member.map.keyType == typeid(i64) ||
                    member.map.keyType == typeid(f32) || member.map.keyType == typeid(f64) ||
                    Razix::RZTypeRegistry::isTypeTriviallySerializable(member.map.keyType)),
                "HashMap key type must be primitive or trivially serializable");

            RAZIX_CORE_ASSERT(
                (member.map.valueType == typeid(u8) || member.map.valueType == typeid(i8) ||
                    member.map.valueType == typeid(u16) || member.map.valueType == typeid(i16) ||
                    member.map.valueType == typeid(u32) || member.map.valueType == typeid(i32) ||
                    member.map.valueType == typeid(u64) || member.map.valueType == typeid(i64) ||
                    member.map.valueType == typeid(f32) || member.map.valueType == typeid(f64) ||
                    Razix::RZTypeRegistry::isTypeTriviallySerializable(member.map.valueType)),
                "HashMap value type must be primitive or trivially serializable");

            if (ar.mode == RZArchiveMode::kWrite) {
                u32 count          = static_cast<u32>(member.map.ops.get_size(reinterpret_cast<const void*>(base + member.offset)));
                u32 keyBytes       = count * member.map.keySize;
                u32 valueBytes     = count * member.map.valueSize;
                u32 keyValuesBytes = keyBytes + valueBytes;

                RAZIX_CORE_ASSERT(ar.heapAllocator != nullptr, "[Serializer] Heap allocator required for HashMap serialization");
                void* keyValuesBuf = ar.heapAllocator->allocate(keyValuesBytes);
                RAZIX_CORE_ASSERT(keyValuesBuf != nullptr, "[Serializer] Heap allocator failed to allocate HashMap buffer");

                member.map.ops.get_data(reinterpret_cast<const void*>(base + member.offset), keyValuesBuf);

                RZSerializedBlob keysValuesBlob = {};
                keysValuesBlob.size             = keyValuesBytes;
                keysValuesBlob.offset           = 0;
                keysValuesBlob.typeHash         = 0;
                keysValuesBlob.compression      = RZ_COMPRESSION_NONE;
                keysValuesBlob.decompressedSize = keyValuesBytes;

                RZSerializedHashMap serializedHashMap = {};
                serializedHashMap.count               = static_cast<u32>(count);
                serializedHashMap.keysValues          = keysValuesBlob;

                size_t headerOffset = ar.cursor;
                ar.write(&serializedHashMap, sizeof(RZSerializedHashMap));

                if constexpr (Archive::kUsesOutOfLineBlobs) {
                    ar.registerWriteBlob(headerOffset, keyValuesBuf, keysValuesBlob.decompressedSize, keysValuesBlob.compression);
                } else {
                    ar.write(keyValuesBuf, keysValuesBlob.size);
                }

                ar.heapAllocator->deallocate(keyValuesBuf);
            } else {
                RZSerializedHashMap serializedHashMap = {};
                ar.read(&serializedHashMap, sizeof(RZSerializedHashMap));

                void* map = reinterpret_cast<void*>(base + member.offset);
                member.map.ops.set_size(map, serializedHashMap.count);

                if constexpr (Archive::kUsesOutOfLineBlobs) {
                    ar.registerReadType(serializedHashMap.keysValues, map, SerializeableDataType::kHashMap, member.map.ops);
                } else {
                    ar.readHashMap(serializedHashMap.keysValues, map, member.map.ops);
                }
            }
        }

        static void processObject(Archive& ar, u8* base, const MemberMetaData& member)
        {
            RAZIX_CORE_ASSERT(member.dataType == SerializeableDataType::kObject, "processObject called for non-object member");
            const TypeMetaData* meta = Razix::RZTypeRegistry::getTypeMetaData(member.object.type);
            RAZIX_CORE_ASSERT(meta != NULL, "Type metadata for object member not found, check if the type is reflected properly");

            if (meta->bIsTriviallySerializable) {
                if (ar.mode == RZArchiveMode::kWrite)
                    ar.write(base, meta->size);
                else
                    ar.read(base, meta->size);
                return;
            }

            RAZIX_CORE_WARN("Non trivial object serialization might not work as expected currently, it's a little buggy");

            for (const auto& member: meta->members)
                processMember(ar, base, member);
        }

        static void processUUID(Archive& ar, u8* base, const MemberMetaData& member)
        {
            RAZIX_CORE_ASSERT(member.uuid.ops.get_data != NULL, "RZUUID get_data function pointer is null");
            RAZIX_CORE_ASSERT(member.uuid.ops.set_data != NULL, "RZUUID ops set_data function pointer is null");

            if (ar.mode == RZArchiveMode::kWrite) {
                // UUID is trivially sized; no out-of-line payload needed, but keep symmetry.
                ar.write(base + member.offset, sizeof(RZUUID));
            } else {
                ar.read(base + member.offset, sizeof(RZUUID));
                RZUUID* uuid = reinterpret_cast<RZUUID*>(base + member.offset);
                member.uuid.ops.set_data(uuid, base + member.offset);
            }
        }

        static void processMember(Archive& ar, void* objectBase, const MemberMetaData& member)
        {
            u8* base = reinterpret_cast<u8*>(objectBase);

            switch (SerializableTypeToDiskTag(member.dataType)) {
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
                case RZDiskTypeTag::kObject:
                    processObject(ar, base + member.offset, member);
                    break;
                case RZDiskTypeTag::kUUID:
                    processUUID(ar, base, member);
                    break;
                default:
                    RAZIX_CORE_ERROR("Work in progress handling other serialization types");
                    RAZIX_DEBUG_BREAK();
                    break;
            }
        }

    public:
        static RZDynamicArray<u8> serializeToBinary(const Derived& instance, Memory::RZHeapAllocator& blobAllocator)
        {
            // with pointer data to write, it's hard to let user give memory
            // As we would not know how much to ask for we use a dynamic array which seems safer,
            // Once we hook up the system allocator into the containers we are eves afer, one less place to refactor
            // So this is accetable to use a RZDynamicArray here we just give the instance and get buffer of data in return.
            RZDynamicArray<u8> buffer = {};

            const TypeMetaData* meta = getTypeMetaData();
            if (!meta) {
                RAZIX_CORE_ERROR("[RZSerializable] Type metadata for type '{}' not found.",
                    typeid(Derived).name());
                return buffer;
            }

            Archive ar(&buffer, 0, RZArchiveMode::kWrite, &blobAllocator);

            if (meta->bIsTriviallySerializable) {
                ar.write(&instance, meta->size);
                return buffer;
            }

            for (const auto& member: meta->members)
                processMember(ar, const_cast<Derived*>(&instance), member);

            return buffer;
        }

        static Derived deserializeFromBinary(const RZDynamicArray<u8>& binary, Memory::RZHeapAllocator& blobAllocator)
        {
            if constexpr (rz_is_same_v<Derived, RZAsset>) {
                RAZIX_CORE_ERROR("Use deserializeAssetFromBinary for RZAsset types");
                RAZIX_DEBUG_BREAK();
            }

            Derived data = {};

            const TypeMetaData* meta = getTypeMetaData();
            if (!meta) {
                RAZIX_CORE_ERROR("[RZSerializable] Type metadata for type '{}' not found.",
                    typeid(Derived).name());
                return data;
            }

            RZDynamicArray<u8> temp = binary;
            Archive            ar(&temp, 0, RZArchiveMode::kRead, const_cast<Memory::RZHeapAllocator*>(&blobAllocator));

            if (meta->bIsTriviallySerializable) {
                ar.read(&data, rz_min<size_t>(meta->size, binary.size()));
                return data;
            }

            for (const auto& member: meta->members)
                processMember(ar, &data, member);

            return data;
        }

        static void* deserializeAssetFromBinary(const RZDynamicArray<u8>& binary, void* pAssetMemory, void* pColdDataMemory)
        {
            RAZIX_CORE_ASSERT(pAssetMemory != NULL, "Asset memory pointer is null");
            RAZIX_CORE_ASSERT(pColdDataMemory != NULL, "Asset cold data memory pointer is null");

            void* pAsset = RZAssetUtilCreateAssetInstanceInPlace(pAssetMemory, pColdDataMemory);
            RAZIX_CORE_ASSERT(pAsset != NULL, "Failed to create RZAsset in provided memory");

            const TypeMetaData* meta = getTypeMetaData();
            if (!meta) {
                RAZIX_CORE_ERROR("[RZSerializable] Type metadata for type '{}' not found.",
                    typeid(Derived).name());
                return pAsset;
            }

            RZDynamicArray<u8> temp = binary;
            Archive            ar(&temp, 0, RZArchiveMode::kRead, NULL);

            if (meta->bIsTriviallySerializable) {
                ar.read(pAsset, rz_min<size_t>(meta->size, binary.size()));    // read into object memory
                return pAsset;
            }

            for (const auto& member: meta->members)
                processMember(ar, pAsset, member);

            return pAsset;
        }

        static RZAsset* deserializeAssetFromBinary(const RZDynamicArray<u8>& binary, RZAsset* pPlaceholderAsset)
        {
            RAZIX_CORE_ASSERT(pPlaceholderAsset != NULL, "Asset memory pointer is null");

            const TypeMetaData* meta = getTypeMetaData();
            if (!meta) {
                RAZIX_CORE_ERROR("[RZSerializable] Type metadata for type '{}' not found.",
                    typeid(Derived).name());
                return pPlaceholderAsset;
            }

            RZDynamicArray<u8> temp = binary;
            Archive            ar(&temp, 0, RZArchiveMode::kRead, NULL);

            if (meta->bIsTriviallySerializable) {
                ar.read(pPlaceholderAsset, rz_min<size_t>(meta->size, binary.size()));    // read into object memory
                return pPlaceholderAsset;
            }

            for (const auto& member: meta->members)
                processMember(ar, pPlaceholderAsset, member);

            return pPlaceholderAsset;
        }

#ifdef RAZIX_USE_COMPRESSED_ARCHIVE
        static RZAsyncSerializationContext beginAsyncSerialization(const Derived& instance, Memory::RZHeapAllocator* blobAllocator)
        {
            RZAsyncSerializationContext ctx = {};
            ctx.mode                        = RZArchiveMode::kWrite;

            const TypeMetaData* meta = getTypeMetaData();
            if (!meta) {
                RAZIX_CORE_ERROR("[RZSerializable] Type metadata for type '{}' not found.",
                    typeid(Derived).name());
                return ctx;
            }

            ctx.archive = Archive(&ctx.write.resultBuffer, 0, ctx.mode, blobAllocator);

            if constexpr (!Archive::kUsesOutOfLineBlobs) {
                RZFileHeader fh{};
                fh.magic              = RAZIX_ASSSET_FILE_MAGIC;
                fh.version            = 0;
                fh.payloadSectionSize = 0;
                fh.headerSectionSize  = 0;
                ctx.archive.write(&fh, sizeof(RZFileHeader));
            }

            if (meta->bIsTriviallySerializable) {
                ctx.archive.write(&instance, meta->size);
                return ctx;
            }

            for (const auto& member: meta->members)
                processMember(ctx.archive, const_cast<Derived*>(&instance), member);

            return ctx;
        }

        static void processAsyncSerialization(RZAsyncSerializationContext& ctx, rz_compression_type compressionType)
        {
            if constexpr (Archive::kUsesOutOfLineBlobs) {
                ctx.archive.compressPayloads(compressionType);
            }
        }

        static void endAsyncSerialization(RZAsyncSerializationContext& ctx)
        {
            if constexpr (Archive::kUsesOutOfLineBlobs) {
                ctx.archive.finalizeWrites();
            }
        }

        static RZAsyncSerializationContext beginAsyncDeserialization(const RZDynamicArray<u8>& binary, Derived* targetInstance, Memory::RZHeapAllocator* blobAllocator = nullptr)
        {
            RAZIX_CORE_ASSERT(targetInstance != nullptr, "Target instance is null");

            RZAsyncSerializationContext ctx = {};
            ctx.mode                        = RZArchiveMode::kRead;
            ctx.read.targetInstance         = targetInstance;
            ctx.read.sourceBuffer           = binary;

            const TypeMetaData* meta = getTypeMetaData();
            if (!meta) {
                RAZIX_CORE_ERROR("[RZSerializable] Type metadata for type '{}' not found.",
                    typeid(Derived).name());
                RZSerializer(yes rename it !) return ctx;
            }

            ctx.archive = Archive(&ctx.read.sourceBuffer, 0, ctx.mode, blobAllocator);

            if constexpr (Archive::kUsesOutOfLineBlobs) {
                RZFileHeader fh{};
                ctx.archive.read(&fh, sizeof(RZFileHeader));
                RAZIX_CORE_ASSERT(fh.magic == RAZIX_ASSSET_FILE_MAGIC, "[Serializer] Bad magic");

                const size_t fileHeaderSize = sizeof(RZFileHeader);
                const size_t headerOffset   = fileHeaderSize;
                const size_t payloadOffset  = fileHeaderSize + fh.headerSectionSize;

                u8* fileBase    = ctx.read.sourceBuffer.data();
                u8* headerBase  = fileBase + headerOffset;
                u8* payloadBase = fileBase + payloadOffset;

                ctx.archive.fileBase    = fileBase;
                ctx.archive.headerBase  = headerBase;
                ctx.archive.payloadBase = payloadBase;
                ctx.archive.mode        = ctx.mode;
                ctx.archive.pendingReadBlobs.clear();
            }

            if (meta->bIsTriviallySerializable) {
                ctx.archive.read(ctx.read.targetInstance, rz_min<size_t>(meta->size, ctx.read.sourceBuffer.size()));
                return ctx;
            }

            for (const auto& member: meta->members)
                processMember(ctx.archive, ctx.read.targetInstance, member);

            return ctx;
        }

        static void processAsyncDeserialization(RZAsyncSerializationContext& ctx)
        {
            if constexpr (Archive::kUsesOutOfLineBlobs) {
                ctx.archive.decompressPayloads();
            }
        }

        static void endAsyncDeserialization(RZAsyncSerializationContext& ctx)
        {
            if constexpr (Archive::kUsesOutOfLineBlobs) {
                ctx.archive.pendingReadBlobs.clear();
            }
        }
#endif    // RAZIX_USE_COMPRESSED_ARCHIVE
    };
}    // namespace Razix
#endif    // _RZ_SERIALIZABLE_H

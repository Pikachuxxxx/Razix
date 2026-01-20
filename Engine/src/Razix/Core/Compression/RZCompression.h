#ifndef _RZ_COMPRESSION_H_
#define _RZ_COMPRESSION_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef enum rz_compression_type : u8
{
    RZ_COMPRESSION_NONE    = 0,
    RZ_COMPRESSION_ZLIB    = 1 << 0,
    RZ_COMPRESSION_LZ4     = 1 << 1,
    RZ_COMPRESSION_BCn     = 1 << 2,
    RZ_COMPRESSION_ASTC    = 1 << 3,
    RZ_COMPRESSION_MESHOPT = 1 << 4,
    RZ_COMPRESSION_FLAC    = 1 << 5,
    RZ_COMPRESSION_COUNT   = 7,
} rz_compression_type;

typedef enum rz_compression_result
{
    RZ_COMPRESSION_OK = 0,
    RZ_COMPRESSION_ERROR,
    RZ_COMPRESSION_INVALID_ARGUMENT,
    RZ_COMPRESSION_BUFFER_TOO_SMALL
} rz_compression_result;

RAZIX_API u32 rz_min_compressed_size(rz_compression_type method, size_t inputSize);
RAZIX_API rz_compression_result rz_compress(rz_compression_type method, const void* inputData, size_t inputSize, void* outputData, size_t* outCompressedSize);
RAZIX_API rz_compression_result rz_decompress(rz_compression_type method, const void* inputData, size_t inputSize, void* outputData, size_t outputSize, size_t* outDecompressedSize);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif    // _RZ_COMPRESSION_H_

#include "RZCompression.h"

#include "Razix/Core/Log/RZLog.h"

#include <lz4.h>

//-----------------------------------------------------------------------------
// LZ4 compression utils
//-----------------------------------------------------------------------------

static rz_compression_result rz_compress_lz4(const void* inputData, size_t inputSize, void* outputData, size_t* outCompressedSize)
{
    if (inputData == NULL || outputData == NULL || outCompressedSize == NULL) {
        RAZIX_CORE_ERROR("Null pointer passed to LZ4 compression");
        return RZ_COMPRESSION_INVALID_ARGUMENT;
    }

    if (inputSize >= LZ4_MAX_INPUT_SIZE) {
        RAZIX_CORE_ERROR("Input size too large for LZ4 compression");
        return RZ_COMPRESSION_INVALID_ARGUMENT;
    }

    u32 maxCompressedSize = LZ4_compressBound(inputSize);
    // we assume the output buffer is large enough to hold the compressed data and it atleast has size of LZ4_compressBound(inputSize)

    u32 compressedSize = LZ4_compress_default(inputData, outputData, inputSize, maxCompressedSize);
    if (compressedSize <= 0) {
        return RZ_COMPRESSION_ERROR;
    }
    *outCompressedSize = compressedSize;
    return RZ_COMPRESSION_OK;
}

static rz_compression_result rz_decompress_lz4(const void* inputData, size_t inputSize, void* outputData, size_t outputSize, size_t* decompressedSize)
{
    if (inputData == NULL || outputData == NULL || decompressedSize == NULL) {
        RAZIX_CORE_ERROR("Null pointer passed to LZ4 decompression");
        return RZ_COMPRESSION_INVALID_ARGUMENT;
    }

    if (outputSize >= LZ4_MAX_INPUT_SIZE) {
        RAZIX_CORE_ERROR("Output size too large for LZ4 decompression");
        return RZ_COMPRESSION_INVALID_ARGUMENT;
    }

    u32 decompressedBytes = LZ4_decompress_safe(inputData, outputData, inputSize, outputSize);

    if (decompressedSize < 0) {
        return RZ_COMPRESSION_ERROR;
    }
    *decompressedSize = decompressedBytes;
    return RZ_COMPRESSION_OK;
}

//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------

u32 rz_min_compressed_size(rz_compression_type algorithm, size_t inputSize)
{
    switch (algorithm) {
        case RZ_COMPRESSION_LZ4:
            return LZ4_compressBound(inputSize);
        default:
            return 0;
    }
}

rz_compression_result rz_compress(rz_compression_type algorithm, const void* inputData, size_t inputSize, void* outputData, size_t* outputSize)
{
    switch (algorithm) {
        case RZ_COMPRESSION_LZ4:
            return rz_compress_lz4(inputData, inputSize, outputData, outputSize);
        default:
            return RZ_COMPRESSION_ERROR;
    }
}

rz_compression_result rz_decompress(rz_compression_type algorithm, const void* inputData, size_t inputSize, void* outputData, size_t outputSize, size_t* outDecompressedSize)
{
    switch (algorithm) {
        case RZ_COMPRESSION_LZ4:
            return rz_decompress_lz4(inputData, inputSize, outputData, outputSize, outDecompressedSize);
        default:
            return RZ_COMPRESSION_ERROR;
    }
}

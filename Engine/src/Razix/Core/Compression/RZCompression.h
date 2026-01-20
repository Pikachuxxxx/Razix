#ifndef _RZ_COMPRESSION_H_
#define _RZ_COMPRESSION_H_

// Razix compression utils

typedef enum RZCompressionFlags : u8
{
    RZ_COMPRESSION_NONE    = 0,
    RZ_COMPRESSION_ZLIB    = 1 << 0,
    RZ_COMPRESSION_LZ4     = 1 << 1,
    RZ_COMPRESSION_BCn     = 1 << 2,
    RZ_COMPRESSION_ASTC    = 1 << 3,
    RZ_COMPRESSION_MESHOPT = 1 << 4,
    RZ_COMPRESSION_FLAC    = 1 << 5,
    RZ_COMPRESSION_COUNT   = 7,
} RZCompressionFlags;

#endif    // _RZ_COMPRESSION_H_
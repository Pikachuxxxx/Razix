// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZUUID.h"

#include <memory>
#include <random>

namespace Razix {

#if defined(BIGENDIAN)
    // Try to use compiler intrinsics
    #if defined(__INTEL_COMPILER) || defined(__ICC)
        #define betole16(x) _bswap16(x)
        #define betole32(x) _bswap(x)
        #define betole64(x) _bswap64(x)
    #elif defined(__GNUC__)    // GCC and CLANG
        #define betole16(x) __builtin_bswap16(x)
        #define betole32(x) __builtin_bswap32(x)
        #define betole64(x) __builtin_bswap64(x)
    #elif defined(_MSC_VER)    // MSVC
        #include <stdlib.h>
        #define betole16(x) _byteswap_ushort(x)
        #define betole32(x) _byteswap_ulong(x)
        #define betole64(x) _byteswap_uint64(x)
    #else
        #define FALLBACK_SWAP
        #define betole16(x) swap_u16(x)
        #define betole32(x) swap_u32(x)
        #define betole64(x) swap_u64(x)
    #endif
    #define betole128(x) swap_u128(x)
    #define betole256(x) swap_u256(x)
#else
    #define betole16(x)  (x)
    #define betole32(x)  (x)
    #define betole64(x)  (x)
    #define betole128(x) (x)
    #define betole256(x) (x)
#endif    // BIGENDIAN

#if defined(FALLBACK_SWAP)
    #include <stdint.h>
    inline u16 swap_u16(u16 value)
    {
        return ((value & 0xFF00u) >> 8u) |
               ((value & 0x00FFu) << 8u);
    }
    inline u32 swap_u32(u32 value)
    {
        return ((value & 0xFF000000u) >> 24u) |
               ((value & 0x00FF0000u) >> 8u) |
               ((value & 0x0000FF00u) << 8u) |
               ((value & 0x000000FFu) << 24u);
    }
    inline uint64_t swap_u64(uint64_t value)
    {
        return ((value & 0xFF00000000000000u) >> 56u) |
               ((value & 0x00FF000000000000u) >> 40u) |
               ((value & 0x0000FF0000000000u) >> 24u) |
               ((value & 0x000000FF00000000u) >> 8u) |
               ((value & 0x00000000FF000000u) << 8u) |
               ((value & 0x0000000000FF0000u) << 24u) |
               ((value & 0x000000000000FF00u) << 40u) |
               ((value & 0x00000000000000FFu) << 56u);
    }
#endif    // FALLBACK_SWAP

    static void m128iToString(__m128i x, char* mem)
    {
        const __m256i mask         = _mm256_set1_epi8(0x0F);
        const __m256i add          = _mm256_set1_epi8(0x06);
        const __m256i alpha_mask   = _mm256_set1_epi8(0x10);
        const __m256i alpha_offset = _mm256_set1_epi8(0x57);

        __m256i a      = _mm256_castsi128_si256(x);
        __m256i as     = _mm256_srli_epi64(a, 4);
        __m256i lo     = _mm256_unpacklo_epi8(as, a);
        __m128i hi     = _mm256_castsi256_si128(_mm256_unpackhi_epi8(as, a));
        __m256i c      = _mm256_inserti128_si256(lo, hi, 1);
        __m256i d      = _mm256_and_si256(c, mask);
        __m256i alpha  = _mm256_slli_epi64(_mm256_and_si256(_mm256_add_epi8(d, add), alpha_mask), 3);
        __m256i offset = _mm256_blendv_epi8(_mm256_slli_epi64(add, 3), alpha_offset, alpha);
        __m256i res    = _mm256_add_epi8(d, offset);

        const __m256i dash_shuffle = _mm256_set_epi32(0x0b0a0908, 0x07060504, 0x80030201, 0x00808080, 0x0d0c800b, 0x0a090880, 0x07060504, 0x03020100);
        const __m256i dash         = _mm256_set_epi64x(0x0000000000000000ull, 0x2d000000002d0000ull, 0x00002d000000002d, 0x0000000000000000ull);

        __m256i resd = _mm256_shuffle_epi8(res, dash_shuffle);
        resd         = _mm256_or_si256(resd, dash);

        _mm256_storeu_si256((__m256i*) mem, betole256(resd));
        *(u16*) (mem + 16) = betole16(_mm256_extract_epi16(res, 7));
        *(u32*) (mem + 32) = betole32(_mm256_extract_epi32(res, 7));
    }

    static __m128i stringTom128i(const char* mem)
    {
        const __m256i dash_shuffle = _mm256_set_epi32(0x80808080, 0x0f0e0d0c, 0x0b0a0908, 0x06050403, 0x80800f0e, 0x0c0b0a09, 0x07060504, 0x03020100);

        __m256i x = betole256(_mm256_loadu_si256((__m256i*) mem));
        x         = _mm256_shuffle_epi8(x, dash_shuffle);
        x         = _mm256_insert_epi16(x, betole16(*(u16*) (mem + 16)), 7);
        x         = _mm256_insert_epi32(x, betole32(*(u32*) (mem + 32)), 7);

        const __m256i sub           = _mm256_set1_epi8(0x2F);
        const __m256i mask          = _mm256_set1_epi8(0x20);
        const __m256i alpha_offset  = _mm256_set1_epi8(0x28);
        const __m256i digits_offset = _mm256_set1_epi8(0x01);
        const __m256i unweave       = _mm256_set_epi32(0x0f0d0b09, 0x0e0c0a08, 0x07050301, 0x06040200, 0x0f0d0b09, 0x0e0c0a08, 0x07050301, 0x06040200);
        const __m256i shift         = _mm256_set_epi32(0x00000000, 0x00000004, 0x00000000, 0x00000004, 0x00000000, 0x00000004, 0x00000000, 0x00000004);

        __m256i a        = _mm256_sub_epi8(x, sub);
        __m256i alpha    = _mm256_slli_epi64(_mm256_and_si256(a, mask), 2);
        __m256i sub_mask = _mm256_blendv_epi8(digits_offset, alpha_offset, alpha);
        a                = _mm256_sub_epi8(a, sub_mask);
        a                = _mm256_shuffle_epi8(a, unweave);
        a                = _mm256_sllv_epi32(a, shift);
        a                = _mm256_hadd_epi32(a, _mm256_setzero_si256());
        a                = _mm256_permute4x64_epi64(a, 0b00001000);

        return _mm256_castsi256_si128(a);
    }
}    // namespace Razix

extern "C"
{
    rz_uuid rz_uuid_generate()
    {
        rz_uuid                                 res;
        static std::random_device               rd;
        static std::mt19937_64                  generator(rd());
        std::uniform_int_distribution<uint64_t> distribution(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());

        const __m128i and_mask = _mm_set_epi64x(0xFFFFFFFFFFFFFF3Full, 0xFF0FFFFFFFFFFFFFull);
        const __m128i or_mask  = _mm_set_epi64x(0x0000000000000080ull, 0x0040000000000000ull);
        __m128i       n        = _mm_set_epi64x(distribution(generator), distribution(generator));
        __m128i       uuid     = _mm_or_si128(_mm_and_si128(n, and_mask), or_mask);

        _mm_storeu_si128(((__m128i*) res.data), uuid);
        return res;
    }

    rz_uuid rz_uuid_from_str(const char* str)
    {
        rz_uuid res;
        __m128i x = Razix::stringTom128i(str);
        _mm_storeu_si128((__m128i*) res.data, x);
        return res;
    }

    rz_uuid rz_uuid_from_pretty_str(const char* prettyStr)
    {
        rz_uuid res;
        size_t  len = strlen(prettyStr);
        if (len != 36) {
            memset(res.data, 0, 16);
            return res;
        }

        size_t byteIndex = 0;
        for (size_t i = 0; i < len; ++i) {
            if (prettyStr[i] == '-') continue;

            char hexByte[3]       = {prettyStr[i], prettyStr[i + 1], '\0'};
            res.data[byteIndex++] = static_cast<uint8_t>(std::strtol(hexByte, nullptr, 16));
            ++i;
        }
        return res;
    }

    void rz_uuid_to_bytes(const rz_uuid* uuid, char* out_bytes)
    {
        __m128i x = betole128(_mm_loadu_si128((const __m128i*) uuid->data));
        _mm_storeu_si128((__m128i*) out_bytes, x);
    }

    void rz_uuid_to_pretty_str(const rz_uuid* uuid, char* out_str)
    {
        __m128i x = _mm_loadu_si128((const __m128i*) uuid->data);
        Razix::m128iToString(x, out_str);
        out_str[36] = '\0';
    }

    uint64_t rz_uuid_hash(const rz_uuid* uuid)
    {
        const uint64_t a = *((const uint64_t*) uuid->data);
        const uint64_t b = *((const uint64_t*) &uuid->data[8]);
        return a ^ b + 0x9e3779b9 + (a << 6) + (a >> 2);
    }

    int rz_uuid_compare(const rz_uuid* a, const rz_uuid* b)
    {
        const uint64_t* x = (const uint64_t*) a->data;
        const uint64_t* y = (const uint64_t*) b->data;

        if (x[0] < y[0]) return -1;
        if (x[0] > y[0]) return 1;
        if (x[1] < y[1]) return -1;
        if (x[1] > y[1]) return 1;
        return 0;
    }

}    // extern "C"

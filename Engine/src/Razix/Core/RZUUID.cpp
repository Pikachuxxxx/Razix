// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZUUID.h"

// Only use simde if we use clang on windows --> soon to use only 256b-it AVX as PS5 doesn't support avx-512

#ifdef __clang__ 
    #define SIMDE_ENABLE_NATIVE_ALIASES
    #define SIMDE_X86_AVX_ENABLE_NATIVE_ALIASES
    
    #include <simde/x86/sse4.1.h>
    #include <simde/x86/avx.h>  // AVX
    #include <simde/x86/avx2.h>  // AVX
#endif

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

#if defined(BIGENDIAN)

    inline __m128i swap_u128(__m128i value)
    {
        const __m128i shuffle = _mm_set_epi64x(0x0001020304050607, 0x08090a0b0c0d0e0f);
        return _mm_shuffle_epi8(value, shuffle);
    }

    inline __m256i swap_u256(__m256i value)
    {
        const __m256i shuffle = _mm256_set_epi64x(0x0001020304050607, 0x08090a0b0c0d0e0f, 0x0001020304050607, 0x08090a0b0c0d0e0f);
        return _mm256_shuffle_epi8(value, shuffle);
    }
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

    //---------------------------------------------------------------------------------------------------------
    // RZUUID Class
    //---------------------------------------------------------------------------------------------------------

    RZUUID::RZUUID()
    {

        /**
         * Section 4.4 
         * 
         * The version 4 UUID is meant for generating UUIDs from truly-random or pseudo-random numbers
         * The algorithm is as follows : 
         * 
         * 1. Set the two most significant bits (bits 6 and 7) of the clock_seq_hi_and_reserved to zero and one, respectively.
         * 2. Set the four most significant bits (bits 12 through 15) of the time_hi_and_version field to the 4-bit version number from Section 4.1.3.
         * 3. Set all the other bits to randomly (or pseudo-randomly) chosen values.
         * 
         * Section 4.1.3
         * 
         * The version number is in the most significant 4 bits of the time stamp (bits 4 through 7 of the time_hi_and_version field)
         * 0     1     0     0 - V4 UUID The randomly or pseudo-randomly generated version UUID
         * 
         */
        // Automatically generate a UUID
        std::shared_ptr<std::mt19937_64>        generator = std::make_shared<std::mt19937_64>(std::random_device()());
        std::uniform_int_distribution<uint64_t> distribution(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());
        // The two masks set the uuid version (4) and variant (1)
        const __m128i and_mask = _mm_set_epi64x(0xFFFFFFFFFFFFFF3Full, 0xFF0FFFFFFFFFFFFFull);
        const __m128i or_mask  = _mm_set_epi64x(0x0000000000000080ull, 0x0040000000000000ull);
        __m128i       n        = _mm_set_epi64x(distribution(*generator), distribution(*generator));
        __m128i       uuid     = _mm_or_si128(_mm_and_si128(n, and_mask), or_mask);

        // Store 128-bits of integer data from a into memory aligned at 16-byte boundary
        _mm_storeu_si128(((__m128i*) m_Data), uuid);
    }

    RZUUID::RZUUID(const RZUUID& other)
    {
        // Load 128-bits of integer data from memory into 128-bit register destination memory aligned at 16-byte boundary
        __m128i x = _mm_loadu_si128((__m128i*) other.m_Data);
        _mm_storeu_si128((__m128i*) m_Data, x);
    }

    RZUUID::RZUUID(__m128i uuid)
    {
        _mm_storeu_si128((__m128i*) m_Data, uuid);
    }

    RZUUID::RZUUID(uint64_t x, uint64_t y)
    {
        // _mm_set_epi64x sets x as first 64 bits and y as the next 64 bits and returns the 128-bit register contents
        __m128i z = _mm_set_epi64x(x, y);
        _mm_storeu_si128((__m128i*) m_Data, z);
    }

    RZUUID::RZUUID(const u8* bytes)
    {
        __m128i x = _mm_loadu_si128((__m128i*) bytes);
        _mm_storeu_si128((__m128i*) m_Data, x);
    }

    RZUUID::RZUUID(const std::string& bytes)
    {
        __m128i x = betole128(_mm_loadu_si128((__m128i*) bytes.data()));
        _mm_storeu_si128((__m128i*) m_Data, x);
    }

    RZUUID RZUUID::FromStrFactory(const std::string& s)
    {
        return FromStrFactory(s.c_str());
    }

    RZUUID RZUUID::FromStrFactory(cstr raw)
    {
        return RZUUID(stringTom128i(raw));
    }

    std::string RZUUID::bytes() const
    {
        std::string mem;
        bytes(mem);
        return mem;
    }

    void RZUUID::bytes(std::string& out) const
    {
        out.resize(sizeof(m_Data));
        bytes((char*) out.data());
    }

    void RZUUID::bytes(char* bytes) const
    {
        __m128i x = betole128(_mm_loadu_si128((__m128i*) m_Data));
        _mm_storeu_si128((__m128i*) bytes, x);
    }

    std::string RZUUID::prettyString() const
    {
        std::string mem;
        prettyString(mem);
        return mem;
    }

    void RZUUID::prettyString(std::string& s) const
    {
        s.resize(36);
        prettyString((char*) s.data());
    }

    void RZUUID::prettyString(char* res) const
    {
        __m128i x = _mm_loadu_si128((__m128i*) m_Data);
        m128iToString(x, res);
    }

    sz RZUUID::hash() const
    {
        // Simple hash function for this class
        //return *((uint64_t*) data) ^ *((uint64_t*) data + 8);
        // https://github.com/crashoz/uuid_v4/pull/13
        const uint64_t a = *((uint64_t*) m_Data);
        const uint64_t b = *((uint64_t*) &m_Data[8]);
        return a ^ b + 0x9e3779b9 + (a << 6) + (a >> 2);
    }

    RZUUID& RZUUID::operator=(const RZUUID& other)
    {
        if (&other == this) {
            return *this;
        }
        __m128i x = _mm_loadu_si128((__m128i*) other.m_Data);
        _mm_storeu_si128((__m128i*) m_Data, x);
        return *this;
    }

    void RZUUID::m128iToString(__m128i x, char* mem)
    {
        // Expand each byte in x to two bytes in res
        // i.e. 0x12345678 -> 0x0102030405060708
        // Then translate each byte to its hex ascii representation
        // i.e. 0x0102030405060708 -> 0x3132333435363738
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

        // Add dashes between blocks as specified in RFC-4122
        // 8-4-4-4-12
        const __m256i dash_shuffle = _mm256_set_epi32(0x0b0a0908, 0x07060504, 0x80030201, 0x00808080, 0x0d0c800b, 0x0a090880, 0x07060504, 0x03020100);
        const __m256i dash         = _mm256_set_epi64x(0x0000000000000000ull, 0x2d000000002d0000ull, 0x00002d000000002d, 0x0000000000000000ull);

        __m256i resd = _mm256_shuffle_epi8(res, dash_shuffle);
        resd         = _mm256_or_si256(resd, dash);

        _mm256_storeu_si256((__m256i*) mem, betole256(resd));
        *(u16*) (mem + 16) = betole16(_mm256_extract_epi16(res, 7));
        *(u32*) (mem + 32) = betole32(_mm256_extract_epi32(res, 7));
    }

    __m128i RZUUID::stringTom128i(cstr mem)
    {
        // Remove dashes and pack hex ascii bytes in a 256-bits int
        const __m256i dash_shuffle = _mm256_set_epi32(0x80808080, 0x0f0e0d0c, 0x0b0a0908, 0x06050403, 0x80800f0e, 0x0c0b0a09, 0x07060504, 0x03020100);

        __m256i x = betole256(_mm256_loadu_si256((__m256i*) mem));
        x         = _mm256_shuffle_epi8(x, dash_shuffle);
        x         = _mm256_insert_epi16(x, betole16(*(u16*) (mem + 16)), 7);
        x         = _mm256_insert_epi32(x, betole32(*(u32*) (mem + 32)), 7);

        // Build a mask to apply a different offset to alphas and digits
        const __m256i sub           = _mm256_set1_epi8(0x2F);
        const __m256i mask          = _mm256_set1_epi8(0x20);
        const __m256i alpha_offset  = _mm256_set1_epi8(0x28);
        const __m256i digits_offset = _mm256_set1_epi8(0x01);
        const __m256i unweave       = _mm256_set_epi32(0x0f0d0b09, 0x0e0c0a08, 0x07050301, 0x06040200, 0x0f0d0b09, 0x0e0c0a08, 0x07050301, 0x06040200);
        const __m256i shift         = _mm256_set_epi32(0x00000000, 0x00000004, 0x00000000, 0x00000004, 0x00000000, 0x00000004, 0x00000000, 0x00000004);

        // Translate ascii bytes to their value
        // i.e. 0x3132333435363738 -> 0x0102030405060708
        // Shift hi-digits
        // i.e. 0x0102030405060708 -> 0x1002300450067008
        // Horizontal add
        // i.e. 0x1002300450067008 -> 0x12345678
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
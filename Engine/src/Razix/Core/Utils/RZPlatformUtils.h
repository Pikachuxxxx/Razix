#pragma once

#if defined(RAZIX_ARCHITECTURE_X64) || defined(RAZIX_ARCHITECTURE_X86)
    #define RAZIX_HAS_X86_INTRINSICS 1

    #if defined(RAZIX_ARCHITECTURE_X64)
        #define RAZIX_GUARANTEED_SSE2 1
    #endif

    // MSVC-specific detection
    #if defined(_MSC_VER)
        // MSVC x64 guarantees SSE2
        #if defined(_M_X64)
            #define RAZIX_COMPILER_SSE  1
            #define RAZIX_COMPILER_SSE2 1
        #endif

        // Check MSVC /arch flags
        #if defined(__AVX512F__)    // This might not be defined in MSVC
            #define RAZIX_COMPILER_AVX512 1
        #endif
        #if defined(__AVX2__) || defined(_M_X64)    // AVX2 available on x64
            #define RAZIX_COMPILER_AVX2 1
        #endif
        #if defined(__AVX__) || defined(_M_X64)    // AVX available on x64
            #define RAZIX_COMPILER_AVX 1
        #endif

        // SSE is standard on x64, and available with /arch:SSE on x86
        #if defined(_M_X64) || defined(_M_IX86_FP)
            #define RAZIX_COMPILER_SSE4_2 1
            #define RAZIX_COMPILER_SSE4_1 1
            #define RAZIX_COMPILER_SSSE3  1
            #define RAZIX_COMPILER_SSE3   1
        #endif

    #else
        // GCC/Clang detection (original code)
        #if defined(__AVX512F__)
            #define RAZIX_COMPILER_AVX512 1
        #endif
        #if defined(__AVX2__)
            #define RAZIX_COMPILER_AVX2 1
        #endif
        #if defined(__AVX__)
            #define RAZIX_COMPILER_AVX 1
        #endif
        #if defined(__SSE4_2__)
            #define RAZIX_COMPILER_SSE4_2 1
        #endif
        #if defined(__SSE4_1__)
            #define RAZIX_COMPILER_SSE4_1 1
        #endif
        #if defined(__SSSE3__)
            #define RAZIX_COMPILER_SSSE3 1
        #endif
        #if defined(__SSE3__)
            #define RAZIX_COMPILER_SSE3 1
        #endif
        #if defined(__SSE2__)
            #define RAZIX_COMPILER_SSE2 1
        #endif
        #if defined(__SSE__)
            #define RAZIX_COMPILER_SSE 1
        #endif
    #endif

#elif defined(RAZIX_ARCHITECTURE_ARM64) || defined(RAZIX_ARCHITECTURE_ARM)
    // ARM detection remains the same
    #define RAZIX_HAS_ARM_INTRINSICS 1

    #if defined(RAZIX_ARCHITECTURE_ARM64)
        #define RAZIX_GUARANTEED_NEON  1
        #define RAZIX_GUARANTEED_ASIMD 1
    #endif

    #if defined(__ARM_NEON) || defined(__ARM_NEON__)
        #define RAZIX_COMPILER_NEON 1
    #endif
#endif

#if defined(RAZIX_HAS_X86_INTRINSICS)
    #if defined(RAZIX_PLATFORM_WINDOWS)
        #include <intrin.h>
    #else
        #include <cpuid.h>
    #endif
#elif defined(RAZIX_HAS_ARM_INTRINSICS)
    #if defined(RAZIX_PLATFORM_WINDOWS)
        #include <processthreadsapi.h>
    #elif defined(RAZIX_PLATFORM_LINUX) || defined(RAZIX_PLATFORM_FREEBSD)
        #include <asm/hwcap.h>
        #include <sys/auxv.h>
    #elif defined(RAZIX_PLATFORM_MACOS)
        #include <sys/sysctl.h>
    #endif
#endif

namespace Razix {

    enum class Endianess
    {
        LITTLE,
        BIG
    };

    typedef enum SIMD
    {
        SIMD_NONE   = 0,
        SIMD_SSE    = 1 << 0,
        SIMD_SSE2   = 1 << 1,
        SIMD_SSE3   = 1 << 2,
        SIMD_SSSE3  = 1 << 3,
        SIMD_SSE4_1 = 1 << 4,
        SIMD_SSE4_2 = 1 << 5,
        SIMD_AVX    = 1 << 6,
        SIMD_AVX2   = 1 << 7,
        SIMD_AVX512 = 1 << 8,
        SIMD_NEON   = 1 << 9,    // ARM NEON support
        SIMD_ASIMD  = 1 << 10    // ARMv8 ASIMD
    } SIMD;

    Endianess GetEndianess();

    u32  DetectSIMDSupport();
    bool HasSIMDSupport(SIMD feature);

    // clang-format off
    static inline u32 GetSIMDSupport() { return DetectSIMDSupport(); }

    static inline bool IsLittleEndian() { return GetEndianess() == Endianess::LITTLE; }
    static inline bool IsBigEndian() { return GetEndianess() == Endianess::BIG; }

    static inline bool HasSSE() { return GetSIMDSupport() & SIMD_SSE; }
    static inline bool HasSSE2() { return GetSIMDSupport() & SIMD_SSE2; }
    static inline bool HasSSE3() { return GetSIMDSupport() & SIMD_SSE3; }
    static inline bool HasSSSE3() { return GetSIMDSupport() & SIMD_SSSE3; }
    static inline bool HasSSE41() { return GetSIMDSupport() & SIMD_SSE4_1; }
    static inline bool HasSSE42() { return GetSIMDSupport() & SIMD_SSE4_2; }

    static inline bool HasAVX() { return GetSIMDSupport() & SIMD_AVX; }
    static inline bool HasAVX2() { return GetSIMDSupport() & SIMD_AVX2; }
    static inline bool HasAVX512() { return GetSIMDSupport() & SIMD_AVX512; }

    static inline bool HasNEON() { return GetSIMDSupport() & SIMD_NEON; }
    static inline bool HasASIMD() { return GetSIMDSupport() & SIMD_ASIMD; }
    // clang-format on
}    // namespace Razix
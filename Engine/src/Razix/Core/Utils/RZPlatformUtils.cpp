// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZPlatformUtils.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <Windows.h>
#elif RAZIX_PLATFORM_LINUX
    #include <pthread.h>
#endif

namespace Razix {

    Endianess GetEndianess()
    {
        // Create a 2-byte integer with a known pattern
        u16 value = 0x01;

        // Use a pointer to examine the first byte
        u8* bytePtr = reinterpret_cast<u8*>(&value);

        // If the first byte is 0x01, it means the machine is little-endian, since the least significant byte is stored first
        if (*bytePtr == 0x01)
            return Endianess::LITTLE;
        else
            return Endianess::BIG;
    }

    bool HasSIMDSupport(SIMD feature)
    {
        return (GetSIMDSupport() & feature) != 0;
    }

#if defined(RAZIX_HAS_X86_INTRINSICS)
    u32 DetectX86SIMDSupport()
    {
        u32 simdFlags = SIMD_NONE;

    #if defined(RAZIX_PLATFORM_WINDOWS)
        int cpuInfo[4];

        // Check basic CPUID support
        __cpuid(cpuInfo, 0);
        int maxLeaf = cpuInfo[0];

        if (maxLeaf >= 1) {
            __cpuid(cpuInfo, 1);
            int ecx = cpuInfo[2];
            int edx = cpuInfo[3];

            // EDX flags
            if (edx & (1 << 25)) simdFlags |= SIMD_SSE;
            if (edx & (1 << 26)) simdFlags |= SIMD_SSE2;

            // ECX flags
            if (ecx & (1 << 0)) simdFlags |= SIMD_SSE3;
            if (ecx & (1 << 9)) simdFlags |= SIMD_SSSE3;
            if (ecx & (1 << 19)) simdFlags |= SIMD_SSE4_1;
            if (ecx & (1 << 20)) simdFlags |= SIMD_SSE4_2;
            if (ecx & (1 << 28)) simdFlags |= SIMD_AVX;

            // Check XSAVE support for AVX
            if ((ecx & (1 << 27)) && (ecx & (1 << 28))) {
                // Check OS support for AVX
                u64 xcr0 = _xgetbv(0);
                if ((xcr0 & 0x6) == 0x6) {
                    simdFlags |= SIMD_AVX;

                    // Check AVX2
                    if (maxLeaf >= 7) {
                        __cpuidex(cpuInfo, 7, 0);
                        if (cpuInfo[1] & (1 << 5)) simdFlags |= SIMD_AVX2;
                        if (cpuInfo[1] & (1 << 16)) simdFlags |= SIMD_AVX512;
                    }
                }
            }
        }

    #elif defined(RAZIX_PLATFORM_LINUX) || defined(RAZIX_PLATFORM_MACOS) || defined(RAZIX_PLATFORM_FREEBSD)
        unsigned int eax, ebx, ecx, edx;

        // Check basic CPUID support
        if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
            // EDX flags
            if (edx & bit_SSE) simdFlags |= SIMD_SSE;
            if (edx & bit_SSE2) simdFlags |= SIMD_SSE2;

            // ECX flags
            if (ecx & bit_SSE3) simdFlags |= SIMD_SSE3;
            if (ecx & bit_SSSE3) simdFlags |= SIMD_SSSE3;
            if (ecx & bit_SSE4_1) simdFlags |= SIMD_SSE4_1;
            if (ecx & bit_SSE4_2) simdFlags |= SIMD_SSE4_2;

            // Check AVX support
            if ((ecx & bit_OSXSAVE) && (ecx & bit_AVX)) {
                // Check OS support for AVX
                asm volatile("xgetbv" : "=a"(eax), "=d"(edx) : "c"(0));
                if ((eax & 0x6) == 0x6) {
                    simdFlags |= SIMD_AVX;

                    // Check AVX2
                    if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
                        if (ebx & bit_AVX2) simdFlags |= SIMD_AVX2;
                        if (ebx & (1 << 16)) simdFlags |= SIMD_AVX512;
                    }
                }
            }
        }
    #endif

        return simdFlags;
    }
#endif    // RAZIX_HAS_X86_INTRINSICS

#if defined(RAZIX_HAS_ARM_INTRINSICS)
    u32 DetectARMSIMDSupport()
    {
        u32 simdFlags = SIMD_NONE;

    #if defined(RAZIX_PLATFORM_WINDOWS)
        // Windows ARM64 - NEON is always available on ARM64
        #if defined(RAZIX_ARCHITECTURE_ARM64)
        simdFlags |= SIMD_NEON | SIMD_ASIMD;
        #else
        // Check via IsProcessorFeaturePresent for ARM32
        if (IsProcessorFeaturePresent(PF_ARM_NEON_INSTRUCTIONS_AVAILABLE)) {
            simdFlags |= SIMD_NEON;
        }
        #endif

    #elif defined(RAZIX_PLATFORM_LINUX) || defined(RAZIX_PLATFORM_FREEBSD)
        // Linux/FreeBSD - check via auxiliary vector
        #if defined(RAZIX_ARCHITECTURE_ARM64)
        simdFlags |= SIMD_NEON | SIMD_ASIMD;    // Always available on ARM64
        #else
        unsigned long hwcap = getauxval(AT_HWCAP);
        if (hwcap & HWCAP_NEON) {
            simdFlags |= SIMD_NEON;
        }
        #endif

    #elif defined(RAZIX_PLATFORM_MACOS)
        // macOS - check via sysctl
        #if defined(RAZIX_ARCHITECTURE_ARM64)
        simdFlags |= SIMD_NEON | SIMD_ASIMD;    // Always available on Apple Silicon
        #else
        // For ARM32 (if ever needed)
        int    hasNEON = 0;
        size_t size    = sizeof(hasNEON);
        if (sysctlbyname("hw.optional.neon", &hasNEON, &size, nullptr, 0) == 0 && hasNEON) {
            simdFlags |= SIMD_NEON;
        }
        #endif
    #endif

        return simdFlags;
    }
#endif    // RAZIX_HAS_ARM_INTRINSICS

    u32 DetectSIMDSupport()
    {
#if defined(RAZIX_HAS_X86_INTRINSICS)
        return DetectX86SIMDSupport();
#elif defined(RAZIX_HAS_ARM_INTRINSICS)
        return DetectARMSIMDSupport();
#else
        return SIMD_NONE;
#endif
    }

}    // namespace Razix

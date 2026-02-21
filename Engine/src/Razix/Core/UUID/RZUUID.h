#pragma once

#include "Razix/Core/RZCore.h"

#include "Razix/Core/Containers/hash_functors.h"
#include "Razix/Core/Containers/string.h"

#if defined(RAZIX_APPLE_SILICON) || defined(RAZIX_PLATFORM_MACOS) || defined(RAZIX_PLATFORM_LINUX_ARM64)
    #include <arm_neon.h>

    #if defined(__clang__) || defined(__GNUC__)
        #define SIMDE_ENABLE_NATIVE_ALIASES
        #define SIMDE_X86_AVX_ENABLE_NATIVE_ALIASES

        #include <simde/arm/neon.h>
        #include <simde/x86/avx.h>     // AVX
        #include <simde/x86/avx2.h>    // AVX
        #include <simde/x86/sse4.1.h>
    #endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct rz_uuid
    {
        uint8_t data[16];
    } rz_uuid;

    RAZIX_API rz_uuid  rz_uuid_generate();
    RAZIX_API rz_uuid  rz_uuid_from_str(const char* str);
    RAZIX_API rz_uuid  rz_uuid_from_pretty_str(const char* str);
    RAZIX_API void     rz_uuid_to_bytes(const rz_uuid* uuid, char* out_bytes);
    RAZIX_API void     rz_uuid_to_pretty_str(const rz_uuid* uuid, char* out_str);
    RAZIX_API uint64_t rz_uuid_hash(const rz_uuid* uuid);
    RAZIX_API int      rz_uuid_compare(const rz_uuid* a, const rz_uuid* b);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
inline bool operator==(const rz_uuid& lhs, const rz_uuid& rhs)
{
    return rz_uuid_compare(&lhs, &rhs) == 0;
}

inline bool operator!=(const rz_uuid& lhs, const rz_uuid& rhs)
{
    return !(lhs == rhs);
}

inline bool operator<(const rz_uuid& lhs, const rz_uuid& rhs)
{
    return rz_uuid_compare(&lhs, &rhs) < 0;
}

namespace Razix {

    template<>
    struct rz_hash<rz_uuid>
    {
        size_t operator()(const rz_uuid& uuid) const
        {
            return static_cast<size_t>(rz_uuid_hash(&uuid));
        }
    };

}    // namespace Razix

namespace std {
    template<>
    struct hash<rz_uuid>
    {
        size_t operator()(const rz_uuid& uuid) const
        {
            return static_cast<size_t>(rz_uuid_hash(&uuid));
        }
    };
}    // namespace std
#endif

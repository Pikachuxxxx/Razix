#ifndef _RZ_HASH_H_
#define _RZ_HASH_H_

#include "Razix/Core/std/type_traits.h"

#include <stddef.h>    // for size_t on linux

// just for the std::type_index hash specialization
#include <typeindex>

// [Source]: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
#define FNV_OFFSET_BASIS 0xcbf29ce484222325UL
#define FNV_PRIME        0x100000001b3UL

#define RZ_HASH_MAP_GROWTH_FACTOR 2

namespace Razix {

    namespace detail {
        template<typename T, typename = void>
        struct MissingHashSpecialization
        {
            static constexpr bool value = true;
        };

        template<typename T>
        struct MissingHashSpecialization<T, rz_enable_if_t<rz_is_integral_v<T>>>
        {
            static constexpr bool value = false;
        };
    }    // namespace detail

    //--------------------------------------------------
    // Hash functors
    //--------------------------------------------------
    // Hash specializations for commonly used types
    // Default implementation for integral types using FNV-1a
    template<typename T>
    struct rz_hash
    {
        static_assert(!detail::MissingHashSpecialization<T>::value,
            "Hash not specialized for this type. See template instantiation above for the type.");

        size_t operator()(T value) const
        {
            if constexpr (!detail::MissingHashSpecialization<T>::value) {
                // [Source]: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
                // Use memcpy to treat any integral as bytes
                uint64_t             hash_val = FNV_OFFSET_BASIS;
                const unsigned char* bytes    = reinterpret_cast<const unsigned char*>(&value);

                for (size_t i = 0; i < sizeof(T); ++i) {
                    hash_val ^= bytes[i];
                    hash_val *= FNV_PRIME;
                }
                return static_cast<size_t>(hash_val);
            }
            return 0;
        }
    };

    template<typename T>
    struct rz_hash<T*>
    {
        size_t operator()(T* ptr) const
        {
            return rz_hash<uint64_t>{}(reinterpret_cast<uint64_t>(ptr));
        }
    };

    template<>
    struct rz_hash<std::type_index>
    {
        size_t operator()(const std::type_index& ti) const
        {
            return ti.hash_code();
        }
    };
}    // namespace Razix
#endif    // _RZ_HASH_H_

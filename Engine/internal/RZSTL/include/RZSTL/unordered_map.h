#ifndef RZSTL_UNORDERED_MAP_H
#define RZSTL_UNORDERED_MAP_H

#include "RZSTL/config.h"

#include <EASTL/unordered_map.h>

namespace Razix {
    // Namespace to switch C++ STL with other STLs
    namespace rzstl {

#if USE_EASTL
        template<typename Key, typename T, typename Hash = eastl::hash<Key>, typename Predicate = eastl::equal_to<Key>, typename Allocator = EASTLAllocatorType, bool bCacheHashCode = false>
        using unordered_map = eastl::unordered_map<Key, T, Hash, Predicate, Allocator, bCacheHashCode>;
#elif

        class unordered_map
        {
        };
#endif

    }    // namespace rzstl
}    // namespace Razix
#endif

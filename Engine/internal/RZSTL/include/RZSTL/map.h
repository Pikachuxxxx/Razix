#ifndef RZSTL_MAP_H
#define RZSTL_MAP_H

#include "RZSTL/config.h"

#include <EASTL/map.h>

namespace Razix {
    // Namespace to switch C++ STL with other STLs
    namespace rzstl {

#if USE_EASTL
        template<typename Key, typename T, typename Compare = eastl::less<Key>, typename Allocator = EASTLAllocatorType>
        using map = eastl::map<Key, T, Compare, Allocator>;
#elif

        class map
        {
        };
#endif

    }    // namespace rzstl
}    // namespace Razix
#endif

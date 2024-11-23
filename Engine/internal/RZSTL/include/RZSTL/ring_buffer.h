#ifndef RZSTL_RING_BUFFER_H
#define RZSTL_RING_BUFFER_H

#include "RZSTL/config.h"

#include <EASTL/bonus/ring_buffer.h>

namespace Razix {
    // Namespace to switch C++ STL with other STLs
    namespace rzstl {

#if USE_EASTL
        template<typename T, typename Container = eastl::vector<T>, typename Allocator = typename Container::allocator_type>
        using ring_buffer = eastl::ring_buffer<T, Container, Allocator>;
#elif

        class ring_buffer
        {
        };
#endif

    }    // namespace rzstl
}    // namespace Razix
#endif

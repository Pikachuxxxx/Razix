#ifndef RZSTL_VECTOR_H
#define RZSTL_VECTOR_H

#include "RZSTL/config.h"

namespace Razix {
    // Namespace to switch C++ STL with other STLs
    namespace rzstl {

        // TODO: add a video vector that uses GPU memory to allocate the elements
#if USE_EASTL
        template<typename T>
        using vector = eastl::vector<T>;
#elif

        class vector
        {
        };
#endif

    }    // namespace rzstl
}    // namespace Razix

#endif

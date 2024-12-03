#pragma once

#include "Razix/Core/RZHandle.h"

#include "Razix/Utilities/TRZSingleton.h"

namespace Razix {
    namespace Gfx {

        // [Source]: https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/custom_memory_pools.html

        class RAZIX_API RZGPUResourcePool
        {
        public:
            /* Initializes the Resource System */
            void StartUp();
            /* Shuts down the Resource System */
            void ShutDown();

            virtual u64 AllocateVideoMemoryFromPoolId(u32 poolIdx) = 0;
        };
    }    // namespace Gfx
}    // namespace Razix

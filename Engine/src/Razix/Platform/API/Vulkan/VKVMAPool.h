#pragma once

#include "Razix/Graphics/Resources/RZGPUResourcePool.h"

namespace Razix {
    namespace Graphics {

        class VKVMAPool : public RZGPUResourcePool
        {
        public:
            u64 AllocateVideoMemoryFromPoolId(u32 poolIdx) override;
        };

    }    // namespace Graphics
}    // namespace Razix
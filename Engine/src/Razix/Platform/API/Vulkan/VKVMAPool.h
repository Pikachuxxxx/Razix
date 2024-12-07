#pragma once

#include "Razix/Gfx/Resources/RZGPUResourcePool.h"

namespace Razix {
    namespace Gfx {

        class VKVMAPool : public RZGPUResourcePool
        {
        public:
            u64 AllocateVideoMemoryFromPoolId(u32 poolIdx) override;
        };

    }    // namespace Gfx
}    // namespace Razix
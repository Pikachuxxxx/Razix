#pragma once

#include "Razix/Core/RZDebugConfig.h"

namespace Razix {
    namespace Graphics {
        class RZSemaphore
        {
        public:
            static RZSemaphore* Create(RZ_DEBUG_NAME_TAG_S_ARG);

            virtual void  Destroy()               = 0;
            virtual void* getAPIHandlePtr(u32 idx) = 0;
        };
    }    // namespace Graphics
}    // namespace Razix
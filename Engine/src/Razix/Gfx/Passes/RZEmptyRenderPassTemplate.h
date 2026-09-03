#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

namespace Razix {
    namespace Gfx {

        class RZEmptyRenderPassTemplate : public IRZPass
        {
        public:
            void addPass(RZFrameGraph& framegraph) override;
            void destroy();
        };
    }    // namespace Gfx
}    // namespace Razix

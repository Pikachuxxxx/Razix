#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

namespace Razix {
    namespace Gfx {

        class RZEmptyRenderPassTemplate : public IRZPass
        {
        public:
            void addPass(RZFrameGraph& framegraph, const RZWorld* world) override;
            void destroy();
        };
    }    // namespace Gfx
}    // namespace Razix

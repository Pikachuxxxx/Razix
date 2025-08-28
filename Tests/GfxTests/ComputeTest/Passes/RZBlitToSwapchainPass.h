#pragma once

#include <Razix.h>

namespace Razix {
    namespace Gfx {

        class RZBlitToSwapchainPass final : public IRZPass
        {
        public:
            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;
        };
    }
}

#pragma once

#include <Razix.h>

namespace Razix {
    namespace Gfx {

        class RZBlitToSwapchainPass final : public IRZPass
        {
        public:
            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

            inline void setBlitTexture(RZFrameGraphResource texture) { m_BlitTexture = texture; }

        private:
            RZFrameGraphResource m_BlitTexture;
        };
    }    // namespace Gfx
}    // namespace Razix

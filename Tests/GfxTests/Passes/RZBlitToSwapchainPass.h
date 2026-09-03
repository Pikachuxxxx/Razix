#pragma once

#include <Razix.h>

namespace Razix {
    namespace Gfx {

        class RZBlitToSwapchainPass final : public IRZPass
        {
        public:
            void addPass(RZFrameGraph& framegraph) override;
            void destroy();

            inline void setBlitTexture(RZFrameGraphResource texture) { m_BlitTexture = texture; }

        private:
            RZFrameGraphResource m_BlitTexture;
        };
    }    // namespace Gfx
}    // namespace Razix

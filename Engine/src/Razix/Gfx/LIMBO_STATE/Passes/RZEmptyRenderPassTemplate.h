#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

#include "Razix/Gfx/Passes/Data/GlobalData.h"

#include "Razix/Gfx/Renderers/RZSystemBinding.h"

namespace Razix {
    namespace Gfx {

        class RZEmptyRenderPassTemplate : public IRZPass
        {
        public:
            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_Pipeline;
        };
    }    // namespace Gfx
}    // namespace Razix

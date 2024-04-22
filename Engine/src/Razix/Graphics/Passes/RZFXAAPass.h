#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

#include "Razix/Graphics/Passes/Data/GlobalData.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

namespace Razix {
    namespace Graphics {

        class RZFXAAPass : public IRZPass
        {
        public:
            void addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_Pipeline;
        };
    }    // namespace Graphics
}    // namespace Razix

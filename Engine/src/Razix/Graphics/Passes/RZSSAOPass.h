#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

namespace Razix {
    namespace Graphics {

#define SSAO_KERNEL_SIZE 64
#define SSAO_NOISE_DIM   4

        class RZSSAOPass : public IRZPass
        {
        public:
            RZSSAOPass() {}
            ~RZSSAOPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_Pipeline;
        };
    }    // namespace Graphics
}    // namespace Razix

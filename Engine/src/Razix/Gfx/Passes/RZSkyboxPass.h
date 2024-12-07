#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

namespace Razix {
    namespace Gfx {

        class RZMesh;

        class RZSkyboxPass : public IRZPass
        {
        public:
            RZSkyboxPass() {}
            ~RZSkyboxPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_Pipeline;
            RZPipelineHandle m_ProceduralPipeline;
        };
    }    // namespace Gfx
}    // namespace Razix

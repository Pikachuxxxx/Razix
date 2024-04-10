#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

namespace Razix {
    namespace Graphics {

        class RZMesh;

        class RZSkyboxPass : public IRZPass
        {
        public:
            RZSkyboxPass() {}
            ~RZSkyboxPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_Pipeline;
            RZPipelineHandle m_ProceduralPipeline;
        };
    }    // namespace Graphics
}    // namespace Razix

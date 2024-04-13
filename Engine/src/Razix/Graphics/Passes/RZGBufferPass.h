#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

#include "Razix/Graphics/Passes/Data/GBufferData.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

namespace Razix {
    namespace Graphics {

        /**
         * Renders the scene onto a GBuffer (pos, normals, depth, metallic, specular etc)
         */
        class RZGBufferPass : public IRZPass
        {
        public:
            RZGBufferPass() = default;
            ~RZGBufferPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_Pipeline;
            RZPipelineHandle m_WireframePipeline;
        };
    }    // namespace Graphics
}    // namespace Razix

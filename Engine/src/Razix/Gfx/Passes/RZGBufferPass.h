#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

namespace Razix {
    namespace Gfx {

        /**
         * Renders the scene onto a GBuffer (pos, normals, depth, metallic, specular etc)
         */
        class RZGBufferPass : public IRZPass
        {
        public:
            RZGBufferPass() = default;
            ~RZGBufferPass() {}

            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_Pipeline;
            RZPipelineHandle m_WireframePipeline;
        };
    }    // namespace Gfx
}    // namespace Razix

#pragma once

#include <Razix.h>

namespace Razix {
    namespace Gfx {

        class RZWaveIntrinsicsTestPass : public IRZPass
        {
        public:
            void addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_Pipeline;
            RZShaderHandle   m_Shader;
        };
    }    // namespace Gfx
}    // namespace Razix

#pragma once

#include <Razix.h>

namespace Razix {
    namespace Gfx {

#if 0
        class RZHelloTriangleTestPass final : public IRZPass
        {
        public:
            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            RZPipelineHandle m_Pipeline;
            RZShaderHandle   m_Shader;
        };
#endif
    }    // namespace Gfx
}    // namespace Razix

#pragma once

#include <Razix.h>

namespace Razix {
    namespace Gfx {

        class RZHelloTextureTestPass final : public IRZPass
        {
        public:
            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            rz_texture_handle  m_TestTextureHandle;
            RZShaderHandle   m_Shader;
            RZPipelineHandle m_Pipeline;
        };
    }    // namespace Gfx
}    // namespace Razix

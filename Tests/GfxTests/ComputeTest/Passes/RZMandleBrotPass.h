#pragma once

#include <Razix.h>

namespace Razix {
    namespace Gfx {

        class RZMandleBrotPass final : public IRZPass
        {
        public:
            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

        private:
            rz_gfx_shader_handle         m_Shader;
            rz_gfx_pipeline_handle       m_Pipeline;
            rz_gfx_root_signature_handle m_RootSigHandle;
            RZFrameGraphResource         m_OutputTexture;
        };
    }    // namespace Gfx
}    // namespace Razix
